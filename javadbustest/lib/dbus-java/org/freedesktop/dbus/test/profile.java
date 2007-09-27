/*
   D-Bus Java Implementation
   Copyright (c) 2005-2006 Matthew Johnson

   This program is free software; you can redistribute it and/or modify it
   under the terms of either the GNU General Public License Version 2 or the
   Academic Free Licence Version 2.1.

   Full licence texts are included in the COPYING file with this program.
*/
package org.freedesktop.dbus.test;

import java.util.Random;
import java.util.HashMap;
import java.util.Vector;

import org.freedesktop.DBus.Peer;
import org.freedesktop.DBus.Introspectable;
import org.freedesktop.dbus.DBusConnection;
import org.freedesktop.dbus.DBusInterface;
import org.freedesktop.dbus.DBusSigHandler;
import org.freedesktop.dbus.UInt32;
import org.freedesktop.dbus.exceptions.DBusException;

class ProfileHandler implements DBusSigHandler<Profiler.ProfileSignal>
{
   public int c = 0;
   public void handle(Profiler.ProfileSignal s)
   {
      if (0 == (c++%profile.SIGNAL_INNER)) System.out.print("-");
   }
}

/**
 * Profiling tests.
 */
public class profile
{
   public static final int SIGNAL_INNER = 100;
   public static final int SIGNAL_OUTER = 100;
   public static final int PING_INNER = 100;
   public static final int PING_OUTER = 100;
   public static final int BYTES = 2000000;
   public static final int INTROSPECTION_OUTER = 100;
   public static final int INTROSPECTION_INNER = 10;
   public static final int STRUCT_OUTER = 100;
   public static final int STRUCT_INNER = 10;
   public static final int LIST_OUTER = 100;
   public static final int LIST_INNER = 10;
   public static final int LIST_LENGTH = 100;
   public static final int MAP_OUTER = 100;
   public static final int MAP_INNER = 10;
   public static final int MAP_LENGTH = 100;
   public static final int ARRAY_OUTER = 100;
   public static final int ARRAY_INNER = 10;
   public static final int ARRAY_LENGTH = 1000;
      
   public static class Log
   {
      private long last;
      private int[] deltas;
      private int current = 0;
      public Log(int size)
      {
         deltas = new int[size];
      }
      public void start()
      {
         last = System.currentTimeMillis();
      }
      public void stop()
      {
         deltas[current] = (int) (System.currentTimeMillis()-last);
         current++;
      }
      public double mean()
      {
         if (0 == current) return 0;
         long sum = 0;
         for (int i = 0; i < current; i++)
            sum+=deltas[i];
         return sum /= current;            
      }
      public long min()
      {
         int m = Integer.MAX_VALUE;
         for (int i = 0; i < current; i++)
            if (deltas[i] < m) m = deltas[i];
         return m;
      }
      public long max()
      {
         int m = 0;
         for (int i = 0; i < current; i++)
            if (deltas[i] > m) m = deltas[i];
         return m;
      }
      public double stddev()
      {
         double mean = mean();
         double sum = 0;
         for (int i=0; i < current; i++)
            sum += (deltas[i]-mean)*(deltas[i]-mean);
         return Math.sqrt(sum / (current-1));
      }
   }
   public static void main(String[] args) throws DBusException
   {
      if (0==args.length) {
         System.out.println("You must specify a profile type.");
         System.out.println("Syntax: profile <pings|arrays|introspect|maps|bytes|lists|structs|signals>");
         System.exit(1);
      }
      DBusConnection conn = DBusConnection.getConnection(DBusConnection.SESSION);
      conn.requestBusName("org.freedesktop.DBus.java.profiler");
      if ("pings".equals(args[0])) {
         int count = PING_INNER*PING_OUTER;
         System.out.print("Sending "+count+" pings...");
         Peer p = (Peer) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Peer.class);
         Log l = new Log(count);
         long t = System.currentTimeMillis();
         for (int i = 0; i < PING_OUTER; i++) {
            for (int j = 0; j < PING_INNER; j++) {
               l.start();
               p.Ping();
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
      } else if ("arrays".equals(args[0])) {
         int count = ARRAY_INNER*ARRAY_OUTER;
         System.out.print("Sending array of "+ARRAY_LENGTH+" ints "+count+" times.");
         conn.exportObject("/Profiler", new ProfilerInstance());
         Profiler p = (Profiler) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Profiler.class);
         int[] v = new int[ARRAY_LENGTH];
         Random r = new Random();
         for (int i = 0; i < ARRAY_LENGTH; i++) v[i] = r.nextInt();
         Log l = new Log(count);
         long t = System.currentTimeMillis();
         for (int i = 0; i < ARRAY_OUTER; i++) {
            for (int j = 0; j < ARRAY_INNER; j++) {
               l.start();
               p.array(v);
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
      } else if ("maps".equals(args[0])) {
         int count = MAP_INNER*MAP_OUTER;
         System.out.print("Sending map of "+MAP_LENGTH+" string=>strings "+count+" times.");
         conn.exportObject("/Profiler", new ProfilerInstance());
         Profiler p = (Profiler) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Profiler.class);
         HashMap<String,String> m = new HashMap<String,String>();
         for (int i = 0; i < MAP_LENGTH; i++) 
            m.put(""+i, "hello");
         Log l = new Log(count);
         long t = System.currentTimeMillis();
         for (int i = 0; i < MAP_OUTER; i++) {
            for (int j=0; j < MAP_INNER; j++) {
               l.start();
               p.map(m);
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
      } else if ("lists".equals(args[0])) {
         int count = LIST_OUTER*LIST_INNER;
         System.out.print("Sending list of "+LIST_LENGTH+" strings "+count+" times.");
         conn.exportObject("/Profiler", new ProfilerInstance());
         Profiler p = (Profiler) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Profiler.class);
         Vector<String> v = new Vector<String>();
         for (int i = 0; i < LIST_LENGTH; i++) 
            v.add("hello "+i);
         Log l = new Log(count);
         long t = System.currentTimeMillis();
         for (int i = 0; i < LIST_OUTER; i++) {
            for (int j=0; j < LIST_INNER; j++) {
               l.start();
               p.list(v);
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
      } else if ("structs".equals(args[0])) {
         int count = STRUCT_OUTER*STRUCT_INNER;
         System.out.print("Sending a struct "+count+" times.");
         conn.exportObject("/Profiler", new ProfilerInstance());
         Profiler p = (Profiler) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Profiler.class);
         ProfileStruct ps = new ProfileStruct("hello", new UInt32(18), 500L);
         Log l = new Log(count);
         long t = System.currentTimeMillis();
         for (int i = 0; i < STRUCT_OUTER; i++) {
            for (int j=0; j < STRUCT_INNER; j++) {
               l.start();
               p.struct(ps);
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
      } else if ("introspect".equals(args[0])) {
         int count = INTROSPECTION_OUTER*INTROSPECTION_INNER;
         System.out.print("Recieving introspection data "+count+" times.");
         conn.exportObject("/Profiler", new ProfilerInstance());
         Introspectable is = (Introspectable) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Introspectable.class);
         Log l = new Log(count);
         long t = System.currentTimeMillis();
         String s = null;
         for (int i = 0; i < INTROSPECTION_OUTER; i++) {
            for (int j = 0; j < INTROSPECTION_INNER; j++) {
               l.start();
               s = is.Introspect();
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
         System.out.println("Introspect data: "+s);
      } else if ("bytes".equals(args[0])) {
         System.out.print("Sending "+BYTES+" bytes");
         conn.exportObject("/Profiler", new ProfilerInstance());
         Profiler p = (Profiler) conn.getRemoteObject("org.freedesktop.DBus.java.profiler", "/Profiler", Profiler.class);
         byte[] bs = new byte[BYTES];
         for (int i = 0; i < BYTES; i++) 
            bs[i] = (byte) i;            
         long t = System.currentTimeMillis();
         p.bytes(bs);
         System.out.println(" done in "+(System.currentTimeMillis()-t)+"ms.");
      } else if ("signals".equals(args[0])) {
         int count = SIGNAL_OUTER*SIGNAL_INNER;
         System.out.print("Sending "+count+" signals");
         ProfileHandler ph = new ProfileHandler();
         conn.addSigHandler(Profiler.ProfileSignal.class, ph);
         Log l = new Log(count);
         Profiler.ProfileSignal ps = new Profiler.ProfileSignal("/");
         long t = System.currentTimeMillis();
         for (int i = 0; i < SIGNAL_OUTER; i++) {
            for (int j = 0; j < SIGNAL_INNER; j++) {
               l.start();
               conn.sendSignal(ps);
               l.stop();
            }
            System.out.print(".");
         }
         t = System.currentTimeMillis()-t;
         System.out.println(" done.");
         System.out.println("min/max/avg (ms): "+l.min()+"/"+l.max()+"/"+l.mean());
         System.out.println("deviation: "+l.stddev());
         System.out.println("Total time: "+t+"ms");
         while (ph.c < count) try { Thread.sleep(100); }
         catch (InterruptedException Ie) {};
      } else {
         conn.disconnect();
         System.out.println("Invalid profile ``"+args[0]+"''.");
         System.out.println("Syntax: profile <pings|arrays|introspect|maps|bytes|lists|structs|signals>");
         System.exit(1);
      }
      conn.disconnect();
   }
}
