--Lab2 QUERY 4


SELECT b.visitID, b.menuItemID, m.name AS menuItemName, m.price*b.quantity AS totalItemCost
FROM billEntry AS b
	LEFT JOIN menuItem AS m
		ON b.menuItemID = m.menuItemID
