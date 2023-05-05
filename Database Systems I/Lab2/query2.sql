--Lab2 QUERY 2
--Output the name, description and price of each menuItem that appears on at least one billEntry.The
--attributes in	your result	should appear as theName, theDescription and thePrice. No duplicates should
--appear in	your result

SELECT DISTINCT name AS theName, description AS theDescription, price AS thePrice
FROM menuItem, billEntry
WHERE menuItem.menuItemID = billEntry.menuItemID
