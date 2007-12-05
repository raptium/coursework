/*
 * Evaluation.java
 *
 * Created on November 29, 2007, 12:50 AM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package javanetprobe;
import com.sun.org.apache.xalan.internal.xsltc.compiler.Template;
import java.nio.channels.DatagramChannel;
import java.nio.channels.SocketChannel;
import javax.swing.*;
import java.nio.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.*;
import java.sql.*;
import java.text.SimpleDateFormat;
import java.text.ParseException;
/**
 *
 * @author yjwan6
 */
public class Evaluation {
  
        String Dyear;
        String Dmonth;
        String Dday;
        String Dtime;
        String Ayear;
        String Amonth;
        String Aday;
        String Atime;
        JTextArea jTextArea;
        SimpleDateFormat df;
    public Evaluation(){
        
    }
    public void star() throws ParseException, SQLException {
         df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");
         String starttime=(Dyear+"-"+Dmonth+"-"+Dday+"  "+"00"+":00");
         String overtime=(Ayear+"-"+Amonth+"-"+Aday+"  "+"00"+":00");
         long starttime_long=df.parse(starttime).getTime();
         long overtime_long=df.parse(overtime).getTime();
         connect ConnectObject=new connect();
         
         ResultSet rs=ConnectObject.DoConnect("select sum(fee) from books where IUSSE_TIME <" + overtime_long+"and IUSSE_TIME >"+starttime_long );
         int sale=0;
         while(rs.next()){
              sale=rs.getInt(1);

         }
         //System.out.println(starttime+overtime+sale);
         
         jTextArea.append("\n\\\\\\\\\\"+starttime+"   -   "+overtime+"\\\\\\\\\\\n");
         jTextArea.append(" Total sales : "+new Integer(sale).toString());
         ConnectObject.CloseConnection();
    }
}











