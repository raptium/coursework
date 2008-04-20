/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.ieg4180.jStreamClient.libHTTP;

import java.util.*;

/**
 * The HTTP Header Class
 * @author hguan5
 */
public class HttpHeader {
    private Hashtable<String, String> m_headers;
    
    /**
     * Create a HTTP header
     */
    public HttpHeader(){
        m_headers = new Hashtable<String, String>();
    }
    
    /**
     * Add some a field to the HTTP Header
     * @param field field name
     * @param value field value
     */
    public void add(String field, String value){
        m_headers.put(field, value);
    }
    
    /**
     * Retrieve the value of specified field in the HTTP header
     * if no such field is found, an empty sring will be returned
     * @param field Field name
     * @return value of the specified field
     */
    public String retrieve(String field){
       String v = m_headers.get(field);
       if(v!=null)
           return v;
       else
           return new String("");
    }
    
    /**
     * Get the header as a String
     * @return the string contains all fields in the header
     */
    @Override
    public String toString(){
        String str = null;
        String output = new String();
        for(Object obj: m_headers.keySet()){
            str = (String)obj;
            output += str + ": " + (String)m_headers.get(obj) + "\n";
        }
        return output;
    }

}
