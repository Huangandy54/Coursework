import java.sql.*;
import java.io.*;
import java.util.*;

/**
 * A class that connects to PostgreSQL and disconnects.
 * You will need to change your credentials below, to match the usename and password of your account
 * in the PostgreSQL server.
 * The name of your database in the server is the same as your username.
 * You are asked to include code that tests the methods of the RestaurantApplication class
 * in a similar manner to the sample RunFilmsApplication.java program.
*/


public class RunRestaurantApplication
{
    public static void main(String[] args) {
    	
    	Connection connection = null;
    	try {
    	    //Register the driver
    		Class.forName("org.postgresql.Driver"); 
    	    // Make the connection.
            // You will need to fill in your real username (twice) and password for your
            // Postgres account in the arguments of the getConnection method below.
            connection = DriverManager.getConnection(
                                                     "jdbc:postgresql://cse180-db.lt.ucsc.edu/ahuang44",
                                                     "ahuang44",
                                                     "4684268A");
            
            if (connection != null)
                System.out.println("Connected to the database!");

            /* Include your code below to test the methods of the RestaurantApplication class
             * The sample code in RunFilmsApplication.java should be useful.
             * That code tests other methods for a different database schema.
             * Your code below: */

            /*
			test of getFrequentlyOrderedMenuItems with parameter 65
            */
            RestaurantApplication app = new RestaurantApplication(connection);
            List<Integer> menuIDs = new ArrayList<Integer>();
            menuIDs=app.getFrequentlyOrderedMenuItems(65);
            System.out.println("* Output of getFrequentlyOrderedMenuItems when parameter is 65");

            for (int ids:menuIDs ) {
            	System.out.println(ids);
            }


            /*
			Two tests of updateServerName
			1) serverID 3 and newServerName=Phileas Fogg
			2) serverID 10 and newServerName=John Smith
            */
            //1
			System.out.println("* Output of updateServerName when serverID is 3\n* and newServerName is 'Phileas Fogg'");
			String test1Name="Phileas Fogg";
			int numChanged=app.updateServerName(3,test1Name);
			System.out.println(numChanged);

			//2
			System.out.println("* Output of updateServerName when serverID is 10\n* and newServerName is 'John Smith'");
			String test2Name="John Smith";
			numChanged=app.updateServerName(10,test2Name);
			System.out.println(numChanged);








            /*******************
            * Your code ends here */
            
    	}
    	catch (SQLException | ClassNotFoundException e) {
    		System.out.println("Error while connecting to database: " + e);
    		e.printStackTrace();
    	}
    	finally {
    		if (connection != null) {
    			// Closing Connection
    			try {
					connection.close();
				} catch (SQLException e) {
					System.out.println("Failed to close connection: " + e);
					e.printStackTrace();
				}
    		}
    	}
    }
}
