--Lab3 general.sql
--ahuang44

ALTER TABLE menuItem
ADD CONSTRAINT positive_price CHECK(price>0);

ALTER TABLE Visit
ADD CHECK(custArrive<=custDepart);

ALTER TABLE dinnerTable
ADD CHECK(inUse IS NOT NULL OR numSeats>0);