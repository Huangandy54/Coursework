CREATE OR REPLACE FUNCTION reduceSomeCostsFunction(maxVisitCount integer) RETURNS integer AS $$
DECLARE 
	updated integer := 0;
	custStatus CHAR(1);
	thisCustID integer;
	discount decimal(3,2);
	--cursor will have custID and Status in the order H > M > L while sorted by join date within H or M or L groups
	custCursor CURSOR FOR SELECT custID, status FROM customer WHERE status IN ('H','M','L') ORDER BY CASE status WHEN 'H' THEN 1 WHEN 'M' THEN 2 WHEN 'L' THEN 3 ELSE 5 END, joinDate ASC;

BEGIN
--open cursor
	OPEN custCursor;
	LOOP
		FETCH custCursor INTO thisCustID, custStatus;
		EXIT WHEN NOT FOUND;
		WHILE updated<maxVisitCount LOOP
			CASE custStatus
				WHEN 'H' THEN discount:=0.9;
				WHEN 'M' THEN discount:=0.95;
				WHEN 'L' THEN discount:=0.99;
				ELSE discount:=1;
			END CASE;
			--did not have enough time to figure out how to only update 1 row per customer at a time. LIMIT?
			UPDATE Visit as v SET cost=cost*discount WHERE v.custID = thisCustID;
			updated:= updated +1;
		END LOOP;

	END LOOP;
	CLOSE custCursor;
	RETURN updated;
END;
$$ LANGUAGE plpgsql;
