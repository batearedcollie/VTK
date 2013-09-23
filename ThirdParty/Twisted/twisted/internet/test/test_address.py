# Copyright (c) Twisted Matrix Laboratories.
# See LICENSE for details.

from __future__ import division, absolute_import

import re
import os

from twisted.trial import unittest
from twisted.internet.address import IPv4Address, UNIXAddress, IPv6Address

try:
    os.symlink
except AttributeError:
    symlinkSkip = "Platform does not support symlinks"
else:
    symlinkSkip = None


class AddressTestCaseMixin(object):
    def test_addressComparison(self):
        """
        Two different address instances, sharing the same properties are
        considered equal by C{==} and not considered not equal by C{!=}.

        Note: When applied via UNIXAddress class, this uses the same
        filename for both objects being compared.
        """
        self.assertTrue(self.buildAddress() == self.buildAddress())
        self.assertFalse(self.buildAddress() != self.buildAddress())


    def _stringRepresentation(self, stringFunction):
        """
        Verify that the string representation of an address object conforms to a
        simple pattern (the usual one for Python object reprs) and contains
        values which accurately reflect the attributes of the address.
        """
        addr = self.buildAddress()
        pattern = "".join([
           "^",
           "([^\(]+Address)", # class name,
           "\(",       # opening bracket,
           "([^)]+)",  # arguments,
           "\)",       # closing bracket,
           "$"
        ])
        stringValue = stringFunction(addr)
        m = re.match(pattern, stringValue)
        self.assertNotEquals(
            None, m,
            "%s does not match the standard __str__ pattern "
            "ClassName(arg1, arg2, etc)" % (stringValue,))
        self.assertEqual(addr.__class__.__name__, m.group(1))

        args = [x.strip() for x in m.group(2).split(",")]
        self.assertEqual(
            args,
            [argSpec[1] % (getattr(addr, argSpec[0]),)
             for argSpec in self.addressArgSpec])


    def test_str(self):
        """
        C{str} can be used to get a string representation of an address instance
        containing information about that address.
        """
        self._stringRepresentation(str)


    def test_repr(self):
        """
        C{repr} can be used to get a string representation of an address
        instance containing information about that address.
        """
        self._stringRepresentation(repr)


    def test_hash(self):
        """
        C{__hash__} can be used to get a hash of an address, allowing
        addresses to be used as keys in dictionaries, for instance.
        """
        addr = self.buildAddress()
        d = {addr: True}
        self.assertTrue(d[self.buildAddress()])


    def test_differentNamesComparison(self):
        """
        Check that comparison operators work correctly on address objects
        when a different name is passed in
        """
        self.assertFalse(self.buildAddress() == self.buildDifferentAddress())
        self.assertFalse(self.buildDifferentAddress() == self.buildAddress())

        self.assertTrue(self.buildAddress() != self.buildDifferentAddress())
        self.assertTrue(self.buildDifferentAddress() != self.buildAddress())


    def assertDeprecations(self, testMethod, message):
        """
        Assert that the a DeprecationWarning with the given message was
        emitted against the given method.
        """
        warnings = self.flushWarnings([testMethod])
        self.assertEqual(warnings[0]['category'], DeprecationWarning)
        self.assertEqual(warnings[0]['message'], message)
        self.assertEqual(len(warnings), 1)



class IPv4AddressTestCaseMixin(AddressTestCaseMixin):
    addressArgSpec = (("type", "%s"), ("host", "%r"), ("port", "%d"))



class IPv4AddressTCPTestCase(unittest.SynchronousTestCase,
                             IPv4AddressTestCaseMixin):
    def buildAddress(self):
        """
        Create an arbitrary new L{IPv4Address} instance with a C{"TCP"}
        type.  A new instance is created for each call, but always for the
        same address.
        """
        return IPv4Address("TCP", "127.0.0.1", 0)


    def buildDifferentAddress(self):
        """
        Like L{buildAddress}, but with a different fixed address.
        """
        return IPv4Address("TCP", "127.0.0.2", 0)


    def test_bwHackDeprecation(self):
        """
        If a value is passed for the C{_bwHack} parameter to L{IPv4Address},
        a deprecation warning is emitted.
        """
        # Construct this for warning side-effects, disregard the actual object.
        IPv4Address("TCP", "127.0.0.3", 0, _bwHack="TCP")

        message = (
            "twisted.internet.address.IPv4Address._bwHack is deprecated "
            "since Twisted 11.0")
        return self.assertDeprecations(self.test_bwHackDeprecation, message)



class IPv4AddressUDPTestCase(unittest.SynchronousTestCase,
                             IPv4AddressTestCaseMixin):
    def buildAddress(self):
        """
        Create an arbitrary new L{IPv4Address} instance with a C{"UDP"}
        type.  A new instance is created for each call, but always for the
        same address.
        """
        return IPv4Address("UDP", "127.0.0.1", 0)


    def buildDifferentAddress(self):
        """
        Like L{buildAddress}, but with a different fixed address.
        """
        return IPv4Address("UDP", "127.0.0.2", 0)


    def test_bwHackDeprecation(self):
        """
        If a value is passed for the C{_bwHack} parameter to L{IPv4Address},
        a deprecation warning is emitted.
        """
        # Construct this for warning side-effects, disregard the actual object.
        IPv4Address("UDP", "127.0.0.3", 0, _bwHack="UDP")

        message = (
            "twisted.internet.address.IPv4Address._bwHack is deprecated "
            "since Twisted 11.0")
        return self.assertDeprecations(self.test_bwHackDeprecation, message)



class IPv6AddressTestCase(unittest.SynchronousTestCase, AddressTestCaseMixin):
    addressArgSpec = (("type", "%s"), ("host", "%r"), ("port", "%d"))

    def buildAddress(self):
        """
        Create an arbitrary new L{IPv6Address} instance with a C{"TCP"}
        type.  A new instance is created for each call, but always for the
        same address.
        """
        return IPv6Address("TCP", "::1", 0)


    def buildDifferentAddress(self):
        """
        Like L{buildAddress}, but with a different fixed address.
        """
        return IPv6Address("TCP", "::2", 0)



class UNIXAddressTestCase(unittest.SynchronousTestCase, AddressTestCaseMixin):
    addressArgSpec = (("name", "%r"),)

    def setUp(self):
        self._socketAddress = self.mktemp()
        self._otherAddress = self.mktemp()


    def buildAddress(self):
        """
        Create an arbitrary new L{UNIXAddress} instance.  A new instance is
        created for each call, but always for the same address.
        """
        return UNIXAddress(self._socketAddress)


    def buildDifferentAddress(self):
        """
        Like L{buildAddress}, but with a different fixed address.
        """
        return UNIXAddress(self._otherAddress)


    def test_comparisonOfLinkedFiles(self):
        """
        UNIXAddress objects compare as equal if they link to the same file.
        """
        linkName = self.mktemp()
        self.fd = open(self._socketAddress, 'w')
        os.symlink(os.path.abspath(self._socketAddress), linkName)
        self.assertTrue(
            UNIXAddress(self._socketAddress) == UNIXAddress(linkName))
        self.assertTrue(
            UNIXAddress(linkName) == UNIXAddress(self._socketAddress))
    test_comparisonOfLinkedFiles.skip = symlinkSkip


    def test_hashOfLinkedFiles(self):
        """
        UNIXAddress Objects that compare as equal have the same hash value.
        """
        linkName = self.mktemp()
        self.fd = open(self._socketAddress, 'w')
        os.symlink(os.path.abspath(self._socketAddress), linkName)
        self.assertEqual(
            hash(UNIXAddress(self._socketAddress)), hash(UNIXAddress(linkName)))
    test_hashOfLinkedFiles.skip = symlinkSkip


    def test_bwHackDeprecation(self):
        """
        If a value is passed for the C{_bwHack} parameter to L{UNIXAddress},
        a deprecation warning is emitted.
        """
        # Construct this for warning side-effects, disregard the actual object.
        UNIXAddress(self.mktemp(), _bwHack='UNIX')

        message = (
            "twisted.internet.address.UNIXAddress._bwHack is deprecated "
            "since Twisted 11.0")
        return self.assertDeprecations(self.test_bwHackDeprecation, message)



class EmptyUNIXAddressTestCase(unittest.SynchronousTestCase,
                               AddressTestCaseMixin):
    """
    Tests for L{UNIXAddress} operations involving a C{None} address.
    """
    addressArgSpec = (("name", "%r"),)

    def setUp(self):
        self._socketAddress = self.mktemp()


    def buildAddress(self):
        """
        Create an arbitrary new L{UNIXAddress} instance.  A new instance is
        created for each call, but always for the same address.
        """
        return UNIXAddress(self._socketAddress)


    def buildDifferentAddress(self):
        """
        Like L{buildAddress}, but with a fixed address of C{None}.
        """
        return UNIXAddress(None)


    def test_comparisonOfLinkedFiles(self):
        """
        A UNIXAddress referring to a C{None} address does not compare equal to a
        UNIXAddress referring to a symlink.
        """
        linkName = self.mktemp()
        self.fd = open(self._socketAddress, 'w')
        os.symlink(os.path.abspath(self._socketAddress), linkName)
        self.assertTrue(
            UNIXAddress(self._socketAddress) != UNIXAddress(None))
        self.assertTrue(
            UNIXAddress(None) != UNIXAddress(self._socketAddress))
    test_comparisonOfLinkedFiles.skip = symlinkSkip


    def test_emptyHash(self):
        """
        C{__hash__} can be used to get a hash of an address, even one referring
        to C{None} rather than a real path.
        """
        addr = self.buildDifferentAddress()
        d = {addr: True}
        self.assertTrue(d[self.buildDifferentAddress()])


