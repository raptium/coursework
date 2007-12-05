/*
 * Administrator.java
 *
 * Created on November 15, 2007, 1:00 PM
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
/**
 *
 * @author yjwan6
 */
public class Administrator extends Thread{
    JTextArea jtextarea;
    int choice;
    connect conncetobject;
    
    public Administrator(JTextArea jtextarea,int i ,connect conncetobject) {
        this.conncetobject=conncetobject;
        this.jtextarea=jtextarea;
        this.choice=i;
    }
    
    public void run() {
        try{
            if(choice==1){
                this.AddAll();
            }
            else{
                this.DeleteAll();
            }
        }catch(SQLException e){
            this.jtextarea.append("here Done!\n");
        }
       
    }
    
    public void AddAll() throws SQLException{
       this.predelete();
      

       this.conncetobject.DoConnect("CREATE TABLE flight (id integer,airline varchar(25),depart_city varchar(25), arrival_city varchar(25),depart_time number(20,0),arrival_time number(20,0),price float, num_seats integer,primary key(id))");
       this.conncetobject.DoConnect("create table booking(id integer,p_name varchar(30),from_city varchar(25), to_city varchar(25), depart_after number(20,0),arrival_before number(20,0),p_phone varchar(25), p_ccno varchar(25),primary key(id))");
       this.conncetobject.DoConnect("create table books(bid integer, fid integer, iusse_time number(20,0), fee float)");
           
       SampleDataReader readdata=new SampleDataReader(conncetobject,this.jtextarea);
       readdata.read_bookings();
       readdata.read_books();
       readdata.read_flights();
       conncetobject.con.close();
       this.jtextarea.append("Done\n");
      
        
    }
    public void DeleteAll()throws SQLException{
        this.conncetobject.DoConnect("drop table flight");
        this.conncetobject.DoConnect("drop table booking");
        this.conncetobject.DoConnect("drop table books");
        conncetobject.con.close();
        this.jtextarea.append("Done\n");
    }
    
    private void predelete(){
        this.conncetobject.DoConnect("drop table flight");
        this.conncetobject.DoConnect("drop table booking");
        this.conncetobject.DoConnect("drop table books");
    }
    
}



class Flight{
	public int id;
	public String airline;
	public String depart_city;
	public String arrival_city;
	public long depart_time; //represented in the number of milliseconds since January 1, 1970, 00:00:00 GMT.
	public long arrival_time;
	public float price;
	public int num_seats;
	
	public Flight(){
	}
	public String atoStringTime(){
            SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");
            return df.format(new Date(arrival_time));
        }
        public String dtoStringTime(){
            SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");
            return df.format(new Date(depart_time));
        }
	public String toString(){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
		return ("Airline:" + airline + "\t" + depart_city + " - " + arrival_city + "\t" + df.format(new Date(depart_time)) + " - " + df.format(new Date(arrival_time)) + "\tPrice:" + price + "$\tSeat: " + num_seats);
	}
        
        public String toString2(){
               SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
	       return (id+"\t" + airline + "\t" + depart_city + "\t" + arrival_city + "\t" + df.format(new Date(depart_time)) + "\t" + df.format(new Date(arrival_time)) + "\t" + price + "$\t" + num_seats);
        }
		
}

class Booking{//8 attributes
	public int id;
	public String p_name;
	public String frm_city;
	public String to_city;
	public long depart_after;
	public long arrival_before;
	public String p_phone;
	public String p_ccno;
	
	public Booking(){
	}

	public String toString(){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
		return (id + "\t" + p_name + "\t" + frm_city + "\t" + to_city + "\t" + df.format(new Date(depart_after)) + "\t" + df.format(new Date(arrival_before)) + "\t" + p_phone + "\t" + p_ccno);
	}
}

class Books{//3 attributes
	public int bid;
	public int fid;
	public long issue_time;
	public float fee;
	
	public Books(){
	}

	public String toString(){
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
		return (bid + "\t" + fid + "\t" + df.format(new Date(issue_time)) + "\t" + fee);
	}
}

class SampleDataReader{
        connect conncetobject;
        JTextArea jtextarea;
        public SampleDataReader(connect conncetobject,JTextArea jtextarea){
            this.conncetobject=conncetobject;
            this.jtextarea=jtextarea;
        }
	
	public  void read_flights(){
		Flight flight = new Flight();
		String[] ss=new String[10];
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
		try{
			FileReader fr = new FileReader("Flights.txt"); 
			BufferedReader br = new BufferedReader(fr); 
			String line = br.readLine();
			while(line != null){							
				ss = line.split("\t", 8); //split s by the seperator "\t"
				flight.id = Integer.parseInt(ss[0]);
				flight.airline = ss[1];
				flight.depart_city = ss[2];
				flight.arrival_city = ss[3];
				flight.depart_time = (df.parse(ss[4])).getTime();//parse ss[4] into a Date object, which is then converted into milliseconds using getTime()
				flight.arrival_time = (df.parse(ss[5])).getTime();
				flight.price = Float.parseFloat(ss[6]);
				flight.num_seats = Integer.parseInt(ss[7]);
//id integer,airline varchar(25),depart_city varchar(25), arrival_city varchar(25),depart_time number(20,0),arrival_time number(20,0),price float, num_seats integer,primary key(id))");				
                                PreparedStatement updateSales = conncetobject.con.prepareStatement("insert into flight values(?,?,?,?,?,?,?,?)");
                                updateSales.setInt(1,flight.id); 
                                updateSales.setString(2,flight.airline ); 
                                updateSales.setString(3,flight.depart_city);
                                updateSales.setString(4,flight.arrival_city);
                                updateSales.setLong(5,flight.depart_time);
                                updateSales.setLong(6,flight.arrival_time);
                                updateSales.setFloat(7,flight.price);
                                updateSales.setInt(8,flight.num_seats);
                                updateSales.executeUpdate();
                                //Insert flight into DB...
				//System.out.println(flight.toString());				
				line = br.readLine();
                                updateSales.close();
			}
			br.close();
			fr.close();	
		}catch(IOException e){
			 this.jtextarea.append(e.toString());
                         return ;
		}catch(ParseException e){
			 this.jtextarea.append(e.toString());
                         return ;
		}catch(SQLException e){
                    this.jtextarea.append(e.toString());
                    return ;
                }
	}
	
	public  void read_bookings(){
		Booking booking = new Booking();
		String[] ss=new String[10];
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
		try{
			FileReader fr = new FileReader("Bookings.txt"); 
			BufferedReader br = new BufferedReader(fr); 
			String line = br.readLine();
			while(line != null){							
				ss = line.split("\t", 8); //split s by the seperator "\t"
				booking.id = Integer.parseInt(ss[0]);
				booking.p_name = ss[1];
				booking.frm_city = ss[2];
				booking.to_city = ss[3];
				booking.depart_after = (df.parse(ss[4])).getTime();//parse ss[4] into a Date object, which is then converted into milliseconds using getTime()
				booking.arrival_before = (df.parse(ss[5])).getTime();
				booking.p_phone = ss[6];
				booking.p_ccno = ss[7];
                                
                                PreparedStatement updateSales = conncetobject.con.prepareStatement("insert into booking values(?,?,?,?,?,?,?,?)");
                                updateSales.setInt(1,booking.id); 
                                updateSales.setString(2,booking.p_name ); 
                                updateSales.setString(3,booking.frm_city);
                                updateSales.setString(4,booking.to_city);
                                updateSales.setLong(5,booking.depart_after);
                                updateSales.setLong(6,booking.arrival_before);
                                updateSales.setString(7,booking.p_phone);
                                updateSales.setString(8,booking.p_ccno);
                                updateSales.executeUpdate();
                               // this.conncetobject.DoConnect("insert into booking values("+booking.id+","+booking.p_name+","+booking.frm_city+","+booking.to_city+","+booking.depart_after+","+booking.arrival_before+","+booking.p_phone +","+booking.p_ccno+")");
                                //this.conncetobject.stmt.executeUpdate("insert into booking values("+booking.id+","+booking.p_name+","+booking.frm_city+","+booking.to_city+","+booking.depart_after+","+booking.arrival_before+","+booking.p_phone +","+booking.p_ccno+")");
				//Insert booking into DB...				
				//System.out.println(booking.toString());				
				line = br.readLine();
                                updateSales.close();
			}
                        
			br.close();
			fr.close();	
		}catch(IOException e){
			this.jtextarea.append(e.toString());
                         return ;
		}catch(ParseException e){
			this.jtextarea.append(e.toString());
                         return ;
		}catch(SQLException e){
                        this.jtextarea.append(e.toString());
                         return ;
                }
	}

	public  void read_books(){
		Books books = new Books();
		String[] ss=new String[10];
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd  HH:mm");		
		try{
			FileReader fr = new FileReader("Books.txt"); 
			BufferedReader br = new BufferedReader(fr); 
			String line = br.readLine();
			while(line != null){							
				ss = line.split("\t", 8); //split s by the seperator "\t"
				books.bid = Integer.parseInt(ss[0]);
				books.fid = Integer.parseInt(ss[1]);
				books.issue_time = (df.parse(ss[2])).getTime();//parse ss[4] into a Date object, which is then converted into milliseconds using getTime()				
				books.fee = Float.parseFloat(ss[3]);
				
//(bid integer, fid integer, iusse_time number(20,0), fee float)");//Insert books into DB...		
                                PreparedStatement updateSales = conncetobject.con.prepareStatement("insert into books values(?,?,?,?)");
                                updateSales.setInt(1,books.bid); 
                                updateSales.setInt(2,books.fid); 
                               
                                updateSales.setLong(3,books.issue_time);
                                updateSales.setFloat(4,books.fee);
                                updateSales.executeUpdate();
				//System.out.println(books.toString());
				line = br.readLine();
                                updateSales.close();
			}
			br.close();
			fr.close();	
		}catch(IOException e){
			this.jtextarea.append(e.toString());
                         return ;
		}catch(ParseException e){
			this.jtextarea.append(e.toString());
                         return ;
		}catch(SQLException e){
                    this.jtextarea.append(e.toString());
                         return ;
                }
	}
}
	/*
	public static void main(String[] args) {
		read_flights();
		read_bookings();
		read_books();
	}
}*/
