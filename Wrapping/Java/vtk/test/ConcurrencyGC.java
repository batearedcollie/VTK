package vtk.test;

import java.lang.reflect.InvocationTargetException;
import java.util.concurrent.TimeUnit;

import vtk.vtkJavaGarbageCollector;
import vtk.vtkPoints;
import vtk.vtkUnstructuredGrid;

/**
 * This test run concurrently thread that create Java view of VTK objects and
 * the EDT that collect those objects as well as another thread.
 *
 * Based on the changes done to prevent concurrency during creation/deletion of
 * VTK object this application shouldn't crash.
 *
 * @author sebastien jourdain - sebastien.jourdain@kitware.com
 */
public class ConcurrencyGC {

    public static void main(String[] args) throws InterruptedException, InvocationTargetException {
        try {
            vtkJavaTesting.Initialize(args, true);

            // Setup working runnable
            Runnable workingJob = new Runnable() {
                @Override
                public void run() {
                    try {
                        vtkUnstructuredGrid grid = new vtkUnstructuredGrid();
                        grid.SetPoints(new vtkPoints());
                        vtkPoints p;
                        while (true) {
                            p = grid.GetPoints();
                            if (p == null) {
                                throw new RuntimeException("Invalid pointer null");
                            }
                            if (p.GetReferenceCount() != 2) {
                                throw new RuntimeException("Invalid reference count of " + p.GetReferenceCount());
                            }
                        }
                    } catch (Throwable e) {
                        e.printStackTrace();
                        vtkJavaTesting.Exit(vtkJavaTesting.FAILED);
                    }
                }
            };

            // Start threads for concurrency (2xwork + 1xGC + 1xGCinEDT)
            new Thread(workingJob).start(); // Start working thread
            new Thread(workingJob).start(); // Start working thread

            // Setup GC
            vtkJavaGarbageCollector gc = vtkJavaTesting.StartGCInEDT(10, TimeUnit.MILLISECONDS); // Start periodic GC in EDT
            new Thread(gc.GetDeleteRunnable()).start();                                          // Start GC in tight loop

            // Start timeout exit thread
            vtkJavaTesting.StartTimeoutExit(1, TimeUnit.MINUTES);

        } catch (Throwable e) {
            e.printStackTrace();
            vtkJavaTesting.Exit(vtkJavaTesting.FAILED);
        }
    }
}
