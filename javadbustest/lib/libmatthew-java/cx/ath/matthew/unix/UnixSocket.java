/*
 * Java Unix Sockets Library
 *
 * Copyright (c) Matthew Johnson 2004
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation, version 2 only.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details. 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * To Contact the author, please email src@matthew.ath.cx
 *
 */
package cx.ath.matthew.unix;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Represents a UnixSocket.
 */
public class UnixSocket
{
   static { System.loadLibrary("unix-java"); }
   private native void native_set_pass_cred(int sock, boolean passcred) throws IOException;
   private native int native_connect(String address, boolean abs) throws IOException;
   private native void native_close(int sock) throws IOException;

   private UnixSocketAddress address = null;
   private USOutputStream os = null;
   private USInputStream is = null;
   private boolean closed = false;
   private boolean connected = false;
   private boolean passcred = false;
   private int sock = 0;
   private boolean blocking = true;
   UnixSocket(int sock, UnixSocketAddress address)
   {
      this.sock = sock;
      this.address = address;
      this.connected = true;
      this.os = new USOutputStream(sock, this);
      this.is = new USInputStream(sock, this);
   }
   /**
    * Create an unconnected socket.
    */
   public UnixSocket()
   {
   }
   /**
    * Create a socket connected to the given address.
    * @param address The Unix Socket address to connect to
    */
   public UnixSocket(UnixSocketAddress address) throws IOException
   {
      connect(address);
   }
   /**
    * Create a socket connected to the given address.
    * @param address The Unix Socket address to connect to
    */
   public UnixSocket(String address) throws IOException
   {
      this(new UnixSocketAddress(address));
   }
   /**
    * Connect the socket to this address.
    * @param address The Unix Socket address to connect to
    */
   public void connect(UnixSocketAddress address) throws IOException
   {
      if (connected) close();
      this.sock = native_connect(address.path, address.abs);
      this.os = new USOutputStream(this.sock, this);
      this.is = new USInputStream(this.sock, this);
      this.address = address;
      this.connected = true;
      this.closed = false;
      this.is.setBlocking(blocking);
   }
   /**
    * Connect the socket to this address.
    * @param address The Unix Socket address to connect to
    */
   public void connect(String address) throws IOException
   {
      connect(new UnixSocketAddress(address));
   }
   public void finalize()
   {
      try { 
         close();
      } catch (IOException IOe) {}
   }
   /**
    * Closes the connection.
    */
   public void close() throws IOException
   {
      native_close(sock);
      this.closed = true;
      this.connected = false;
      os = null;
      is = null;
   }
   /**
    * Returns an InputStream for reading from the socket.
    * @return An InputStream connected to this socket.
    */
   public InputStream getInputStream()
   {
      return is;
   }
   /**
    * Returns an OutputStream for writing to the socket.
    * @return An OutputStream connected to this socket.
    */
   public OutputStream getOutputStream()
   {
      return os;
   }
   /**
    * Returns the address this socket is connected to.
    * Returns null if the socket is unconnected.
    * @return The UnixSocketAddress the socket is connected to
    */
   public UnixSocketAddress getAddress()
   { 
      return address;
   }
   /**
    * Get the credential passing status.
    * @return The current status of credential passing.
    * @see setPassCred
    */
   public boolean getPassCred()
   {
      return passcred;
   }
   /**
    * Set the credential passing status.
    * @param enable Set to true for credentials to be passed.
    */
   public void setPassCred(boolean enable) throws IOException
   {
      native_set_pass_cred(sock, enable);
      passcred = enable;
   }
   /**
    * Get the blocking mode.
    * @return true if reads are blocking.
    * @see setBlocking
    */
   public boolean getBlocking()
   {
      return blocking;
   }
   /**
    * Set the blocking mode.
    * @param enable Set to false for non-blocking reads.
    */
   public void setBlocking(boolean enable)
   {
      blocking = enable;
      if (null != is) is.setBlocking(enable);
   }

   /**
    * Check the socket status.
    * @return true if closed.
    */
   public boolean isClosed()
   {
      return closed;
   }
   /**
    * Check the socket status.
    * @return true if connected.
    */
   public boolean isConnected()
   {
      return connected;
   }
   /**
    * Check the socket status.
    * @return true if the input stream has been shutdown
    */
   public boolean isInputShutdown()
   {
      return is.isClosed();
   }
   /**
    * Check the socket status.
    * @return true if the output stream has been shutdown
    */
   public boolean isOutputShutdown()
   {
      return os.isClosed();
   }
   /**
    * Shuts down the input stream.
    * Subsequent reads on the associated InputStream will fail.
    */
   public void shutdownInput()
   {
      is.closed = true;
   }
   /**
    * Shuts down the output stream.
    * Subsequent writes to the associated OutputStream will fail.
    */
   public void shutdownOutput()
   {
      os.closed = true;
   }
   /**
    * Set timeout of read requests.
    */
   public void setSoTimeout(int timeout)
   {
      is.setSoTimeout(timeout);
   }
}
