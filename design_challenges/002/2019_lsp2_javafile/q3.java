import java.io.File;
import java.io.IOException;
import java.io.FileWriter;

public class q3{
	public static void main(String[] args) throws IOException{

		// char *file = "q3java.txt";
		File file = new File("q3java.txt");

		/****** 두번째 매개변수 ******/
		/****** true : 기존 파일의 내용 이후부터 쓰여짐 ******/
		/****** false : 처음부터 쓰여짐 ******/
		// File *writer = fopen(file, "a"); 또는 "w"
		FileWriter writer = new FileWriter(file, false);
		// fprintf(writer, "2019 OSLAB\n");
		writer.write("2019 OSLAB\n");
		writer.write("Linux System Programming\n");
		// fflush(writer);
		writer.flush();
		System.out.printf("DONE\n");
		// writer != NULL
		if(writer != null)
			writer.close(); // fclose(writer);
		
	}
}
