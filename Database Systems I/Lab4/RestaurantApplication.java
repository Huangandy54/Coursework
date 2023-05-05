import java.sql.*;
import java.util.*;

/**
 * The class implements methods of the Restaurant database interface.
 *
 * All methods of the class receive a Connection object through which all
 * communication to the database should be performed. Note: the
 * Connection object should not be closed by any method.
 *
 * Also, no method should throw any exceptions. In particular, in case
 * an error occurs in the database, then the method should print an
 * error message and call System.exit(-1);
 */

public class RestaurantApplication {

    private Connection connection;

    /*
     * Constructor
     */
    public RestaurantApplication(Connection connection) {
        this.connection = connection;
    }

    public Connection getConnection()
    {
        return connection;
    }

    /**
     * getFrequentlyOrderedMenuItems has an integer argument called numMenuItemsOrdered,
     * and returns the menuItemID for each menuItem where the total quantity (adding up quantity 
     * across all billEntry tuples) of that menuItem is greater than or equal to numMenuItemsOrdered.  
     * A value of numMenuItemsOrdered that’s not positive is an error.
     */

    public List<Integer> getFrequentlyOrderedMenuItems(int numMenuItemsOrdered)
    {
        List<Integer> result = new ArrayList<Integer>();
        // your code here
        //return an error and exit if the 
        if (numMenuItemsOrdered<0) {
            System.out.println("getFrequentlyOrderedMenuItems must be called with a positive number");
            System.exit(1);
        }
        try{
            Statement st= connection.createStatement();
            ResultSet rs= st.executeQuery("SELECT menuItemID FROM billEntry GROUP BY menuItemID HAVING SUM(quantity)>="+numMenuItemsOrdered);
            //add each row in the result set to result arraylist 
            while(rs.next()){
                result.add(rs.getInt(1));
            }
        }
        catch(SQLException sqle){
            System.out.println("error running getFrequentlyOrderedMenuItems");
        }

        // end of your code
        return result;
    }


    /**
     * updateServerName method has two arguments, an integer argument, theServerID, and a string 
     * argument, newServerName.  For the tuple in the server table (if any) whose serverID equals 
     * theServerID, updateServerName should update name to be newServerName.  (Note that there may 
     * not be any tuples whose serverID matches theServerID.)  
     * updateServerName should return the number of tuples that were updated, which will always be 0 or 1.
     */

    public int updateServerName(int theServerID, String newServerName)
    {
        // your code here; return 0 appears for now to allow this skeleton to compile.
        int changed =0;
        try{
            PreparedStatement st= connection.prepareStatement("UPDATE server SET name= ? " + "WHERE serverID=?");
            st.setString(1,newServerName);
            st.setInt(2,theServerID);
            changed= st.executeUpdate();
        }
        catch(SQLException sqle){
            System.out.println("error running updateServerName");
        }
        finally {
            if (st != null) st.close();
        }
        return changed;



        // end of your code
    }


    /**
     * reduceSomeVisitCosts has an integer parameters, maxVisitCount.  It invokes a stored function
     * reduceSomeCostsFunction that you will need to implement and store in the database according to the
     * description in Section 5.  reduceSomeCostsFunction should have the same parameters, maxVisitCount.
     * A value of maxVisitCount that’s not positive is an error.
     *
     * The visits table has a cost attribute.  reduceSomeCostsFunction will reduce the cost for some 
     * (but not necessarily all) visits; Section 5 explains which visits should have their cost reduced, 
     * and also tells you how much they should be reduced.  The reduceSomeVisitCosts method should return 
     * the same integer result that the reduceSomeCostsFunction stored function returns.
     *
     * The reduceSomeVisitCosts method must only invoke the stored function reduceSomeCostsFunction, which
     * does all of the assignment work; do not implement the reduceSomeVisitCosts method using a bunch
     * of SQL statements through JDBC.
     */

    public int reduceSomeVisitCosts (int maxVisitCount)
    {
        // There's nothing special about the name storedFunctionResult
        int storedFunctionResult = 0;

        // your code here
        if (maxVisitCount<0) {
            System.out.println("reduceSomeVisitCosts must be called with a positive number");
            System.exit(1);
        }

        
        // end of your code
        return storedFunctionResult;

    }

};
