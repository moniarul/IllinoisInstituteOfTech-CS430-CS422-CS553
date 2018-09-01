import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaRDD;
import org.apache.spark.api.java.JavaSparkContext;
import org.apache.spark.api.java.function.Function;

public class SparkSort {
    public static void main(String[] args) {
        SparkConf conf = new SparkConf().setAppName("Spark Sort").setMaster("yarn");
        JavaSparkContext jsc = new JavaSparkContext(conf);
        long start = System.currentTimeMillis();
        JavaRDD<String> textFile = jsc.textFile(args[0]);
        JavaRDD<String> sorted = textFile.sortBy(new Function<String, String>() {
            public String call(String line) throws Exception {
                return line.substring(0,101);
            }
        }, true, 16);
        String output = args[1];

        sorted.saveAsTextFile(output);

        long end = System.currentTimeMillis();
        long duration = (end - start)/1000;
        System.out.println("Time Taken: " + duration + "s");
    }

}