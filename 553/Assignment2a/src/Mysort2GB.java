import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.LinkedList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.stream.Stream;


class SplitJob extends Thread {
    LinkedList<String> chunkName;
    int startLine, endLine;

    SplitJob(LinkedList<String> chunkName, int startLine, int endLine) {
        this.chunkName = chunkName;
        this.startLine = startLine;
        this.endLine = endLine;
    }

    public void run() {
        try {
            int totalLines = endLine + 1 - startLine;
            Stream<String> chunks =
                    Files.lines(Paths.get(Mysort2GB.fPath))
                            .skip(startLine - 1)
                            .limit(totalLines)
                            .sorted(Comparator.naturalOrder());
            chunks.forEach(line -> {
                chunkName.add(line);
            });
            System.out.println(" Done Writing " + Thread.currentThread().getName());

        } catch (Exception e) {
            System.out.println(e);
        }
    }
}

class MergeJob extends Thread {
    int list1, list2, oplist;
    MergeJob(int list1, int list2, int oplist) {
        this.list1 = list1;
        this.list2 = list2;
        this.oplist = oplist;
    }

    public void run() {
        try {
            System.out.println(list1 + " Started Merging " + list2 );
            LinkedList<String> merged = new LinkedList<>();
            LinkedList<String> ilist1 = Mysort2GB.sortedChunks.get(list1);
            LinkedList<String> ilist2 = Mysort2GB.sortedChunks.get(list2);

            //Merge 2 files based on which string is greater.
            while (ilist1.size() != 0 || ilist2.size() != 0) {
                if (ilist1.size() == 0 ||
                        (ilist2.size() != 0 && ilist1.get(0).compareTo(ilist2.get(0)) > 0)) {
                    merged.add(ilist2.remove(0));
                } else {
                    merged.add(ilist1.remove(0));
                }
            }
            System.out.println(list1 + " Done Merging " + list2 );
            Mysort2GB.sortedChunks.remove(list1);
            Mysort2GB.sortedChunks.remove(list2);
            Mysort2GB.sortedChunks.put(oplist, merged);
        } catch (Exception e) {
            System.out.println(e);
        }
    }
}

public class Mysort2GB {
    public static final String fdir = "/Users/diesel/Desktop/";
    //public static final String fdir = "/tmp/";
    public static final String shared = "/exports/home/schatterjee/cs553-pa2a/";
    public static final String fPath = "/input/data-2GB.in";
    public static HashMap<Integer, LinkedList<String>> sortedChunks = new HashMap();
    public static final String opfile = fdir+"op2GB";
    public static final String opLog = shared + "Mysort2GB.log";


    public static void main(String[] args) throws Exception{
        long startTime = System.nanoTime();
        int threadCount = 8; // Number of threads
        int totalLines = 20000000;
        int linesPerFile = totalLines / threadCount;
        LinkedList<Thread> activeThreads = new LinkedList<Thread>();


        for (int i = 1; i <= threadCount; i++) {
            int startLine = i == 1 ? i : (i - 1) * linesPerFile + 1;
            int endLine = i * linesPerFile;
            LinkedList<String> thisChunk = new LinkedList<>();
            SplitJob mapThreads = new SplitJob(thisChunk, startLine, endLine);
            sortedChunks.put(i,thisChunk);
            activeThreads.add(mapThreads);
            mapThreads.start();
        }
        activeThreads.stream().forEach(t -> {
            try {
                t.join();
            } catch (Exception e) {
            }
        });

        int treeHeight = (int) (Math.log(threadCount) / Math.log(2));

        for (int i = 0; i < treeHeight; i++) {
            LinkedList<Thread> actvThreads = new LinkedList<Thread>();
            for (int j = 1, itr = 1; j <= threadCount / (i + 1); j += 2, itr++) {
                int offset = i * 100;
                int list1 = j + offset;
                int list2 = (j + 1) + offset;
                int opList = itr + ((i + 1) * 100);
                MergeJob reduceThreads =
                        new MergeJob(list1,list2,opList);
                actvThreads.add(reduceThreads);
                reduceThreads.start();
            }
            actvThreads.stream().forEach(t -> {
                try {
                    t.join();
                } catch (Exception e) {
                }
            });
        }
        BufferedWriter writer = Files.newBufferedWriter(Paths.get(opfile));
        sortedChunks.get(treeHeight*100+1).forEach(line -> {
            try {
                writer.write(line+"\r\n");
            }catch (Exception e){

            }
        });
        writer.close();
        long endTime = System.nanoTime();
        double timeTaken = (endTime - startTime)/1e9;
        System.out.println(timeTaken);
        BufferedWriter logFile = new BufferedWriter(new FileWriter(opLog, true));
        logFile.write("Time Taken in seconds:" + timeTaken);
        Runtime.getRuntime().exec("valsort  " + opfile + " > " + opLog).waitFor();
        logFile.close();
    }
}