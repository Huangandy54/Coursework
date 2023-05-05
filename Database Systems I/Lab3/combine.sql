--Lab3 combine.sql
--ahuang44
--insert if VisitID is not already in Visit
--update if VisitID is already in Visit

INSERT INTO Visit(VisitID, custID, dinnerTableID, serverID,numPeople, custArrive)
SELECT m.VisitID, m.custID,m.dinnerTableID,m.serverID, m.numPeople,CURRENT_TIMESTAMP
FROM modifyVisit AS m
WHERE m.VisitID NOT IN (SELECT v.VisitID FROM Visit AS v);

UPDATE Visit
SET custID=m.custID, dinnerTableID=m.dinnerTableID, serverID=m.serverID, numPeople=m.numPeople
from modifyVisit AS m
WHERE Visit.VisitID=m.VisitID;