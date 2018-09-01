import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.io.IntWritable;
import java.util.*;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;

public class HadoopSort {

    public static class StringReducer extends Reducer<Text,Text,Text,Text> {
        Text values=new Text();
        public void reduce(Text MapKey, Iterator<Text>  MapVal, Context ctx) throws IOException, InterruptedException {
            while(MapVal.hasNext())
                values.set(MapVal.next());
            ctx.write(MapKey,values);
        }
    }

    public static class TaskMapper extends Mapper<Object, Text, Text, Text>{

        public Text token1 = new Text();
        public Text token2 = new Text();

        public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
            String val= value.toString();
            token2.set(val.substring(10));
            token1.set(val.substring(0, 10));
            context.write(token1, token2);
        }
    }

    public static void main(String[] args) throws Exception {
        Configuration config = new Configuration();

        long start = System.currentTimeMillis();

        Job job = Job.getInstance(config, "Hadoop Sort Mapreduce");

        job.setJarByClass(HadoopSort.class);

        job.setMapperClass(TaskMapper.class);
        job.setReducerClass(StringReducer.class);

        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(Text.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        long end = System.currentTimeMillis();
        double elapsedTime = (end-start)/1000.0;

        System.out.println("Time taken to sort" + elapsedTime);
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}