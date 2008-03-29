/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.csc4180.jStreamClient;


import java.util.logging.Level;
import java.util.logging.Logger;
import org.cuhk.csc4180.jStreamClient.LibHTTP.*;
/**
 *
 * @author hguan5
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {
            HttpServer server = new HttpServer("localhost", 80);
            server.start();
            while(true){
                try {
                    HttpRequest request = server.getRequest();
                    HttpResponse response = new HttpResponse();
                    
                    response.setStatus("404");
                    String html = new String("");
                    response.fillMessage(html.getBytes());

                    System.out.print(new String(response.toBytes()));
                    server.sendResponse(response);
                } catch (Exception ex){
                    Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
        } catch (Exception ex) {
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        
    }

}
