/*
 * Npopular.java
 *
 * Created on November 28, 2007, 11:45 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package javanetprobe;

import javax.swing.*;
import java.io.*;
import java.text.SimpleDateFormat;
import java.text.ParseException;  
import java.util.Date; 
import java.sql.*;

// create or replace view tocity(tocity) as select a.to_city from booking a where exists (select b.bid from books b    where b.bid=a.id) 
//create or replace view tocity2(tocity,total) as  select tocity , count(tocity) as total from tocity group by tocity order by total desc
/**
 *
 * @author yjwan6
 */ 
       //  create or replace view sale(city,fee) as select a.to_city, b.fee from booking a,books b where a.id=b.bid
public class Npopular {
    
    /** Creates a new instance of Npopular */
    public Npopular(int NO,JTextArea jTextArea) throws SQLException {
       connect ConnectObject=new connect();
       ConnectObject.DoConnect("create or replace view sale(city,fee) as select a.to_city, b.fee from booking a,books b where a.id=b.bid" );
       //ConnectObject.DoConnect("create or replace view tocity2(tocity,total) as  select tocity , count(tocity) as total from tocity group by tocity order by total desc");
       ResultSet rs=ConnectObject.DoConnect("select city, sum(fee) as total from sale group by city order by total desc");
       int a=0;
       jTextArea.append("\n");
       
       while(rs.next()&&a<NO){
            String city=rs.getString(1);
            
            String amount=rs.getString(2);
            jTextArea.append(city+"\t"+amount+"\n");
            a++;
       }
       
       jTextArea.append("\\\\\\\\\\"+a+" Most Popular Cities\\\\\\\\\\");
       ConnectObject.CloseConnection();
    }
    
}
