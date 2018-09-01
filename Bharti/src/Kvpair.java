import java.io.*;
import java.util.HashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

interface KVStore<K, V> {
    void get (K key);
    void put (K key, V value);
    void delete (K key);
    void clear ();
    void size ();
}

class CheckPointIndex extends Thread{
    HashMap<Integer, String> index;
    String identifierPath = "/Users/diesel/Desktop/indexer";
    public CheckPointIndex(HashMap index){
        this.index = index;
    }

    @Override
    public void run() {
        super.run();
        try {
            while (true) {
                ObjectOutputStream ous
                        = new ObjectOutputStream(new FileOutputStream(identifierPath));
                ous.writeObject(index);
                ous.flush();
                ous.close();
                Thread.sleep(1000);
            }
        } catch (Exception e){
            System.out.println("Indexer interrupted");
        }

    }
}

 class Engine implements Runnable{
     HashMap<Integer, String> index;
     // Change this path to your local machine path and manually create s3Bucket1, s3Bucket2.. folders
     // I didnt do it to keep the code succint , and it's not related to this algo...
     String identifierPath = "/Users/diesel/Desktop/";
     String[] distributedMachines = {"s3Bucket1/", "s3Bucket2/", "s3Bucket3/", "s3Bucket4/"};
     String ops,key;
     Object val;

     public Engine(HashMap index, String ops, String key, Object val){
         this.index = index;
        this.ops = ops;
        this.key = key;
        this.val = val;
     }
     @Override
     public void run() {
         int identity = (int)(Thread.currentThread().getId() % KVpair.concurr);
         String location = identifierPath + distributedMachines[identity];
        if(ops.equals("put")){
            int mapval = key.hashCode();
            synchronized (index){
                index.put(mapval, location+mapval);
            }
            try {
                ObjectOutputStream ous
                        = new ObjectOutputStream(new FileOutputStream(location+mapval));
                ous.writeObject(val);
                ous.flush();
                ous.close();
            } catch (Exception e){
                index.remove(mapval);
                System.out.println(e);
            }
            System.out.println(" Stored " + key);

        } else if(ops.equals("get")){
             String serailizedPath = index.get(key.hashCode());
             try {
                 ObjectInputStream ois
                         = new ObjectInputStream(new FileInputStream(serailizedPath));
                 System.out.println("Value is " + ois.readObject());
                 ois.close();
             } catch (Exception e){
                 System.out.println("Value Doesnt exist " + key);
             }

         } else if(ops.equals("delete")){
            synchronized (index) {
                index.remove(key.hashCode());
            }
            // Fire BatchJob that will delete the actual file
        }
     }
 }

class KVpair implements KVStore{
    public static int concurr = 4;
    public static HashMap<Integer, String> index;
    public static ExecutorService executor;
    @Override
    public void get(Object key) {
        executor.execute(new Engine(index,"get", (String) key, null));
    }

    @Override
    public void put(Object key, Object value) {
        executor.execute(new Engine(index,"put", (String) key, value));
    }

    @Override
    public void delete(Object key) {
        executor.execute(new Engine(index,"delete", (String) key, null));
    }

    @Override
    public void clear() {
        index = new HashMap<>();
    }

    @Override
    public void size() {
        System.out.println(" Size is " + index.size());
    }

    public static void main(String[] args) throws Exception{
        KVpair store = new KVpair();
        index = new HashMap();
        CheckPointIndex indexer = new CheckPointIndex(index);
        indexer.start();
        executor = Executors.newFixedThreadPool(concurr);
        for (int i=0; i < 1000; i++){
            store.put("someRandom"+i, "SomeResult"+i);
        }


        Thread.sleep(2000);
        store.get("someRandom50");
        store.get("someRandom335");
        Thread.sleep(2000);
        store.delete("someRandom335");
        executor.execute(new Engine(index,"delete", "abc34", null));
        Thread.sleep(2000);
        store.get("someRandom335");
        Thread.sleep(2000);
        store.size();
        store.clear();
        store.size();

        // Only if I want it to exit ..
        executor.shutdown();
        indexer.interrupt();
    }
}
