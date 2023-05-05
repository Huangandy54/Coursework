--Lab3 unittests.sql
--ahuang44

--FOREIGN KEY VIOLATIONS

--invalid dinnerTableID 9 is not a dinnertableID in dinnertable
INSERT INTO VISIT 
VALUES(100,2,9,2,5,100,CURRENT_TIMESTAMP);

--invalid custID 13 is not a custID in customer table
INSERT INTO VISIT 
VALUES(100,13,8,2,5,100,CURRENT_TIMESTAMP);

--invalid serverID 6 is not a serverID in server table
INSERT INTO VISIT 
VALUES(100,10,8,6,5,100,CURRENT_TIMESTAMP);

--GENERAL CONSTRAINTS VIOLATIONS

--menuItem prices cannot be negative so update to * -1 will be an error
UPDATE menuItem
SET price = price *-1;

--visit assuming data loaded by lab3_data_loading.sql 
--all custDepart values are not before CURRENT_TIMESTAMP thus error
UPDATE visit
SET custArrive=CURRENT_TIMESTAMP;

--dinnerTable
--find a dinnertable that is inUse=NULL and set the seats to a negative number
UPDATE dinnerTable
SET numSeats=-1
WHERE inUse IS NULL;


--GENERAL CONSTRAINTS SUCCESSES

--menuItem double the price wont make it negative
UPDATE menuItem
SET price = price *2;

--visit assuming data loaded by lab3_data_loading.sql 
--changing visit #1's arrivetime to something that is still before the departTime
UPDATE visit
SET custArrive='1500-12-20 08:08:15'
WHERE VisitID=1;

--dinnerTable
--find a dinnertable that is inUse is not null
--double the seats is not a negative number
UPDATE dinnerTable
SET numSeats=numSeats*2
WHERE inUse IS NOT NULL;