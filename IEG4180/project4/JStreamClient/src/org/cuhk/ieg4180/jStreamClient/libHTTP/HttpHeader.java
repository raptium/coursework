/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cuhk.ieg4180.jStreamClient.libHTTP;

import java.util.*;

/**
 *
 * @author hguan5
 */
public class HttpHeader {
    private Hashtable<String, String> m_headers;
    
    
    public HttpHeader(){
        m_headers = new Hashtable<String, String>();
    }
    
    public void add(String field, String value){
        m_headers.put(field, value);
    }
    
    public String retrieve(String field){
       String v = m_headers.get(field);
       if(v!=null)
           return v;
       else
           return new String("");
    }
    
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
