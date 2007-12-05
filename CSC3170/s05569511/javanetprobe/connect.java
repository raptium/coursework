/*
 * Connection.java
 *
 * Created on November 15, 2007, 10:51 PM
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package javanetprobe;//
import java.sql.*;
//import java.sql.*;
/**
 *
 * @author yjwan6
 */

public class connect {
    ResultSet rs;
    Connection con;
    Statement stmt;
    public connect() {
        try{
        Class.forName("oracle.jdbc.driver.OracleDriver"); 
        con= DriverManager.getConnection("jdbc:oracle:thin:@db00.cse.cuhk.edu.hk:1521:db00","db001","core2duo");
        stmt= con.createStatement();
        }catch(ClassNotFoundException e){
           System.out.print(e.toString());
           System.exit(1);
        }
        catch(SQLException e){
           System.out.print(e.toString());
           System.exit(1);
        }
    }
    
    public ResultSet DoConnect(String query){
     
        try{
            
            rs = stmt.executeQuery(query);     
        }catch(SQLException e){
            System.out.println("here "+e.toString());
            
            //return null;
        }
        
        return rs;
    }
    
    public void CloseConnection(){
        try{
        this.stmt.close();
        this.con.close();
       // ;
        }catch(SQLException e){
            System.out.print(e.toString());
            System.exit(1);
        }
    }
}



   
