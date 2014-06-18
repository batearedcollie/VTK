# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.

"""
Tests for L{twisted.python.threadpool}
"""

from __future__ import division, absolute_import

import pickle, time, weakref, gc, threading

from twisted.python.compat import _PY3
from twisted.trial import unittest
from twisted.python import threadpool, threadable, failure, context

#
# See the end of this module for the remainder of the imports.
#

class Synchronization(object):
    failures = 0

    def __init__(self, N, waiting):
        self.N = N
        self.waiting = waiting
        self.lock = threading.Lock()
        self.runs = []

    def run(self):
        # This is the testy part: this is supposed to be invoked
        # serially from multiple threads.  If that is actually the
        # case, we will never fail to acquire this lock.  If it is
        # *not* the case, we might get here while someone else is
        # holding the lock.
        if self.lock.acquire(False):
            if not len(self.runs) % 5:
                time.sleep(0.0002) # Constant selected based on
                                   # empirical data to maximize the
                                   # chance of a quick failure if this
                                   # code is broken.
            self.lock.release()
        else:
            self.failures += 1

        # This is just the only way I can think of to wake up the test
        # method.  It doesn't actually have anything to do with the
        # test.
        self.lock.acquire()
        self.runs.append(None)
        if len(self.runs) == self.N:
            self.waiting.release()
        self.lock.release()

    synchronized = ["run"]
threadable.synchronize(Synchronization)



class ThreadPoolTestCase(unittest.SynchronousTestCase):
    """
    Test threadpools.
    """

    def getTimeout(self):
        """
        Return number of seconds to wait before giving up.
        """
        return 5 # Really should be order of magnitude less


    def _waitForLock(self, lock):
        # We could just use range(), but then we use an extra 30MB of memory
        # on Python 2:
        if _PY3:
            items = range(1000000)
        else:
            items = xrange(1000000)
        for i in items:
            if lock.acquire(False):
                break
            time.sleep(1e-5)
        else:
            self.fail("A long time passed without succeeding")


    def test_attributes(self):
        """
        L{ThreadPool.min} and L{ThreadPool.max} are set to the values passed to
        L{ThreadPool.__init__}.
        """
        pool = threadpool.ThreadPool(12, 22)
        self.assertEqual(pool.min, 12)
        self.assertEqual(pool.max, 22)


    def test_start(self):
        """
        L{ThreadPool.start} creates the minimum number of threads specified.
        """
        pool = threadpool.ThreadPool(0, 5)
        pool.start()
        self.addCleanup(pool.stop)
        self.assertEqual(len(pool.threads), 0)

        pool = threadpool.ThreadPool(3, 10)
        self.assertEqual(len(pool.threads), 0)
        pool.start()
        self.addCleanup(pool.stop)
        self.assertEqual(len(pool.threads), 3)


    def test_adjustingWhenPoolStopped(self):
        """
        L{ThreadPool.adjustPoolsize} only modifies the pool size and does not
        start new workers while the pool is not running.
        """
        pool = threadpool.ThreadPool(0, 5)
        pool.start()
        pool.stop()
        pool.adjustPoolsize(2)
        self.assertEqual(len(pool.threads), 0)


    def test_threadCreationArguments(self):
        """
        Test that creating threads in the threadpool with application-level
        objects as arguments doesn't results in those objects never being
        freed, with the thread maintaining a reference to them as long as it
        exists.
        """
        tp = threadpool.ThreadPool(0, 1)
        tp.start()
        self.addCleanup(tp.stop)

        # Sanity check - no threads should have been started yet.
        self.assertEqual(tp.threads, [])

        # Here's our function
        def worker(arg):
            pass
        # weakref needs an object subclass
        class Dumb(object):
            pass
        # And here's the unique object
        unique = Dumb()

        workerRef = weakref.ref(worker)
        uniqueRef = weakref.ref(unique)

        # Put some work in
        tp.callInThread(worker, unique)

        # Add an event to wait completion
        event = threading.Event()
        tp.callInThread(event.set)
        event.wait(self.getTimeout())

        del worker
        del unique
        gc.collect()
        self.assertEqual(uniqueRef(), None)
        self.assertEqual(workerRef(), None)


    def test_threadCreationArgumentsCallInThreadWithCallback(self):
        """
        As C{test_threadCreationArguments} above, but for
        callInThreadWithCallback.
        """

        tp = threadpool.ThreadPool(0, 1)
        tp.start()
        self.addCleanup(tp.stop)

        # Sanity check - no threads should have been started yet.
        self.assertEqual(tp.threads, [])

        # this holds references obtained in onResult
        refdict = {} # name -> ref value

        onResultWait = threading.Event()
        onResultDone = threading.Event()

        resultRef = []

        # result callback
        def onResult(success, result):
            onResultWait.wait(self.getTimeout())
            refdict['workerRef'] = workerRef()
            refdict['uniqueRef'] = uniqueRef()
            onResultDone.set()
            resultRef.append(weakref.ref(result))

        # Here's our function
        def worker(arg, test):
            return Dumb()

        # weakref needs an object subclass
        class Dumb(object):
            pass

        # And here's the unique object
        unique = Dumb()

        onResultRef = weakref.ref(onResult)
        workerRef = weakref.ref(worker)
        uniqueRef = weakref.ref(unique)

        # Put some work in
        tp.callInThreadWithCallback(onResult, worker, unique, test=unique)

        del worker
        del unique
        gc.collect()

        # let onResult collect the refs
        onResultWait.set()
        # wait for onResult
        onResultDone.wait(self.getTimeout())

        self.assertEqual(uniqueRef(), None)
        self.assertEqual(workerRef(), None)

        # XXX There's a race right here - has onResult in the worker thread
        # returned and the locals in _worker holding it and the result been
        # deleted yet?

        del onResult
        gc.collect()
        self.assertEqual(onResultRef(), None)
        self.assertEqual(resultRef[0](), None)


    def test_persistence(self):
        """
        Threadpools can be pickled and unpickled, which should preserve the
        number of threads and other parameters.
        """
        pool = threadpool.ThreadPool(7, 20)

        self.assertEqual(pool.min, 7)
        self.assertEqual(pool.max, 20)

        # check that unpickled threadpool has same number of threads
        copy = pickle.loads(pickle.dumps(pool))

        self.assertEqual(copy.min, 7)
        self.assertEqual(copy.max, 20)


    def _threadpoolTest(self, method):
        """
        Test synchronization of calls made with C{method}, which should be
        one of the mechanisms of the threadpool to execute work in threads.
        """
        # This is a schizophrenic test: it seems to be trying to test
        # both the callInThread()/dispatch() behavior of the ThreadPool as well
        # as the serialization behavior of threadable.synchronize().  It
        # would probably make more sense as two much simpler tests.
        N = 10

        tp = threadpool.ThreadPool()
        tp.start()
        self.addCleanup(tp.stop)

        waiting = threading.Lock()
        waiting.acquire()
        actor = Synchronization(N, waiting)

        for i in range(N):
            method(tp, actor)

        self._waitForLock(waiting)

        self.failIf(actor.failures, "run() re-entered %d times" %
                                    (actor.failures,))


    def test_callInThread(self):
        """
        Call C{_threadpoolTest} with C{callInThread}.
        """
        return self._threadpoolTest(
            lambda tp, actor: tp.callInThread(actor.run))


    def test_callInThreadException(self):
        """
        L{ThreadPool.callInThread} logs exceptions raised by the callable it
        is passed.
        """
        class NewError(Exception):
            pass

        def raiseError():
            raise NewError()

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThread(raiseError)
        tp.start()
        tp.stop()

        errors = self.flushLoggedErrors(NewError)
        self.assertEqual(len(errors), 1)


    def test_callInThreadWithCallback(self):
        """
        L{ThreadPool.callInThreadWithCallback} calls C{onResult} with a
        two-tuple of C{(True, result)} where C{result} is the value returned
        by the callable supplied.
        """
        waiter = threading.Lock()
        waiter.acquire()

        results = []

        def onResult(success, result):
            waiter.release()
            results.append(success)
            results.append(result)

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThreadWithCallback(onResult, lambda: "test")
        tp.start()

        try:
            self._waitForLock(waiter)
        finally:
            tp.stop()

        self.assertTrue(results[0])
        self.assertEqual(results[1], "test")


    def test_callInThreadWithCallbackExceptionInCallback(self):
        """
        L{ThreadPool.callInThreadWithCallback} calls C{onResult} with a
        two-tuple of C{(False, failure)} where C{failure} represents the
        exception raised by the callable supplied.
        """
        class NewError(Exception):
            pass

        def raiseError():
            raise NewError()

        waiter = threading.Lock()
        waiter.acquire()

        results = []

        def onResult(success, result):
            waiter.release()
            results.append(success)
            results.append(result)

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThreadWithCallback(onResult, raiseError)
        tp.start()

        try:
            self._waitForLock(waiter)
        finally:
            tp.stop()

        self.assertFalse(results[0])
        self.assertTrue(isinstance(results[1], failure.Failure))
        self.assertTrue(issubclass(results[1].type, NewError))


    def test_callInThreadWithCallbackExceptionInOnResult(self):
        """
        L{ThreadPool.callInThreadWithCallback} logs the exception raised by
        C{onResult}.
        """
        class NewError(Exception):
            pass

        waiter = threading.Lock()
        waiter.acquire()

        results = []

        def onResult(success, result):
            results.append(success)
            results.append(result)
            raise NewError()

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThreadWithCallback(onResult, lambda : None)
        tp.callInThread(waiter.release)
        tp.start()

        try:
            self._waitForLock(waiter)
        finally:
            tp.stop()

        errors = self.flushLoggedErrors(NewError)
        self.assertEqual(len(errors), 1)

        self.assertTrue(results[0])
        self.assertEqual(results[1], None)


    def test_callbackThread(self):
        """
        L{ThreadPool.callInThreadWithCallback} calls the function it is
        given and the C{onResult} callback in the same thread.
        """
        threadIds = []

        event = threading.Event()

        def onResult(success, result):
            threadIds.append(threading.currentThread().ident)
            event.set()

        def func():
            threadIds.append(threading.currentThread().ident)

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThreadWithCallback(onResult, func)
        tp.start()
        self.addCleanup(tp.stop)

        event.wait(self.getTimeout())
        self.assertEqual(len(threadIds), 2)
        self.assertEqual(threadIds[0], threadIds[1])


    def test_callbackContext(self):
        """
        The context L{ThreadPool.callInThreadWithCallback} is invoked in is
        shared by the context the callable and C{onResult} callback are
        invoked in.
        """
        myctx = context.theContextTracker.currentContext().contexts[-1]
        myctx['testing'] = 'this must be present'

        contexts = []

        event = threading.Event()

        def onResult(success, result):
            ctx = context.theContextTracker.currentContext().contexts[-1]
            contexts.append(ctx)
            event.set()

        def func():
            ctx = context.theContextTracker.currentContext().contexts[-1]
            contexts.append(ctx)

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThreadWithCallback(onResult, func)
        tp.start()
        self.addCleanup(tp.stop)

        event.wait(self.getTimeout())

        self.assertEqual(len(contexts), 2)
        self.assertEqual(myctx, contexts[0])
        self.assertEqual(myctx, contexts[1])


    def test_existingWork(self):
        """
        Work added to the threadpool before its start should be executed once
        the threadpool is started: this is ensured by trying to release a lock
        previously acquired.
        """
        waiter = threading.Lock()
        waiter.acquire()

        tp = threadpool.ThreadPool(0, 1)
        tp.callInThread(waiter.release) # before start()
        tp.start()

        try:
            self._waitForLock(waiter)
        finally:
            tp.stop()


    def test_workerStateTransition(self):
        """
        As the worker receives and completes work, it transitions between
        the working and waiting states.
        """
        pool = threadpool.ThreadPool(0, 1)
        pool.start()
        self.addCleanup(pool.stop)

        # sanity check
        self.assertEqual(pool.workers, 0)
        self.assertEqual(len(pool.waiters), 0)
        self.assertEqual(len(pool.working), 0)

        # fire up a worker and give it some 'work'
        threadWorking = threading.Event()
        threadFinish = threading.Event()

        def _thread():
            threadWorking.set()
            threadFinish.wait()

        pool.callInThread(_thread)
        threadWorking.wait()
        self.assertEqual(pool.workers, 1)
        self.assertEqual(len(pool.waiters), 0)
        self.assertEqual(len(pool.working), 1)

        # finish work, and spin until state changes
        threadFinish.set()
        while not len(pool.waiters):
            time.sleep(0.0005)

        # make sure state changed correctly
        self.assertEqual(len(pool.waiters), 1)
        self.assertEqual(len(pool.working), 0)


    def test_workerState(self):
        """
        Upon entering a _workerState block, the threads unique identifier is
        added to a stateList and is removed upon exiting the block.
        """
        pool = threadpool.ThreadPool()
        workerThread = object()
        stateList = []
        with pool._workerState(stateList, workerThread):
            self.assertIn(workerThread, stateList)
        self.assertNotIn(workerThread, stateList)


    def test_workerStateExceptionHandling(self):
        """
        The _workerState block does not consume L{Exception}s or change the
        L{Exception} that gets raised.
        """
        pool = threadpool.ThreadPool()
        workerThread = object()
        stateList = []
        try:
            with pool._workerState(stateList, workerThread):
                self.assertIn(workerThread, stateList)
                1 / 0
        except ZeroDivisionError:
            pass
        except:
            self.fail("_workerState shouldn't change raised exceptions")
        else:
            self.fail("_workerState shouldn't consume exceptions")
        self.assertNotIn(workerThread, stateList)



class RaceConditionTestCase(unittest.SynchronousTestCase):

    def getTimeout(self):
        """
        Return number of seconds to wait before giving up.
        """
        return 5 # Really should be order of magnitude less


    def setUp(self):
        self.event = threading.Event()
        self.threadpool = threadpool.ThreadPool(0, 10)
        self.threadpool.start()


    def tearDown(self):
        del self.event
        self.threadpool.stop()
        del self.threadpool


    def test_synchronization(self):
        """
        Test a race condition: ensure that actions run in the pool synchronize
        with actions run in the main thread.
        """
        timeout = self.getTimeout()
        self.threadpool.callInThread(self.event.set)
        self.event.wait(timeout)
        self.event.clear()
        for i in range(3):
            self.threadpool.callInThread(self.event.wait)
        self.threadpool.callInThread(self.event.set)
        self.event.wait(timeout)
        if not self.event.isSet():
            self.event.set()
            self.fail("Actions not synchronized")


    def test_singleThread(self):
        """
        The submission of a new job to a thread pool in response to the
        C{onResult} callback does not cause a new thread to be added to the
        thread pool.

        This requires that the thread which calls C{onResult} to have first
        marked itself as available so that when the new job is queued, that
        thread may be considered to run it.  This is desirable so that when
        only N jobs are ever being executed in the thread pool at once only
        N threads will ever be created.
        """
        # Ensure no threads running
        self.assertEqual(self.threadpool.workers, 0)

        event = threading.Event()
        event.clear()

        def onResult(success, counter):
            event.set()

        for i in range(10):
            self.threadpool.callInThreadWithCallback(
                onResult, lambda: None)
            event.wait()
            event.clear()

        self.assertEqual(self.threadpool.workers, 1)
