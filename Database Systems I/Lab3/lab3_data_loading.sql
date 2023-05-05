-- Sample Script file to Populate a Restaurant DB

-- Populate the tables


-- customer(custID, name, address, joinDate, status)
COPY Customer FROM stdin USING DELIMITERS '|' NULL 'null';
1|Indiana Jones|USA|1973-04-22|H
2|Sherlock Holmes|221 Baker St|1895-04-22|O
3|Dunston|222 Baker St|1895-04-22|O
4|Kevin|223 Baker St|1895-04-22|H
5|Matthew|224 Baker St|1895-04-22|H
6|Maven|null|1995-03-23|O
7|Gavin|null|null|H
8|Matthew|441 Acme Ln|1983-02-03|H
9|Alice|441 Acme Ln|1983-02-03|O
10|Morgan|121 Downside Ln|1991-05-29|H
\.

-- menuItem(menuItemID, name, description, price)
COPY MenuItem FROM stdin USING DELIMITERS '|' NULL 'null';
1|Foie Gras|Something delicious| 112.23
2|Rice bowl|Something delicious| 2.23
3|Ramen bowl|Something delicious| 232.23
4|Fried Rice|Something delicious|14.25
5|Pita Bread|null|4.25
6|Hummus|null|1.25
7|Apple Pie|null|3.25
\.

-- dinnerTable(dinnerTableID, numSeats, InUse)
COPY DinnerTable FROM stdin USING DELIMITERS '|' NULL 'null';
1|4|False
2|8|True
3|2|T
4|8|F
5|12|null
6|2|T
7|16|F
8|6|F
\.

-- server(serverID, name, level, salary)
COPY Server FROM stdin USING DELIMITERS '|' NULL 'null';
1|Bill Jones|T|4322.22
2|Kelly Arthur|null|322.22
3|Fred Flintstone|P|522.22
4|Barney Rubbles|null|522.22
5|Tony Stark|P|900.10
\.

-- visit(visitID, custID, dinnerTableID, serverID, numPeople, cost, custArrive, custDepart)
COPY Visit FROM stdin USING DELIMITERS '|' NULL 'null';
1|1|1|1|3|950.00|'2018-12-20 08:08:15'| '2018-12-20 09:09:01'
2|1|2|2|4|950.00|'2018-12-20 09:13:01'| '2018-12-20 10:13:01'
3|2|4|3|5|950.00|'2018-12-20 12:13:01'| '2018-12-20 14:13:01'
4|6|3|4|2|259.00|'2019-02-20 11:23:01'| '2019-02-20 13:00:01'
5|5|5|3|12|1553.00|null|null
6|2|5|3|9|53.00|null|'2019-05-20 15:30:01'
7|2|4|3|8|150.00|'2018-12-19 12:13:01'| '2018-12-19 14:13:01'
8|2|4|3|7|228.00|'2018-12-18 11:13:01'| '2018-12-18 12:13:01'
9|3|4|3|6|45.00|'2018-12-18 08:13:01'| '2018-12-18 09:13:01'
10|7|2|1|8|134.00|'2018-12-18 07:13:01'| '2018-12-18 09:13:01'
11|1|2|3|3|950.00|'2018-12-19 17:08:15'| '2018-12-19 18:09:01'
\.

-- billEntry(visitID, menuItemID, quantity)
COPY BillEntry FROM stdin USING DELIMITERS '|';
1|1|2
1|2|1
1|3|1
2|1|2
2|2|1
2|3|1
3|4|5
3|3|4
4|1|1
4|2|2
5|5|6
5|6|2
5|4|2
6|3|2
6|2|1
7|1|1
7|2|2
8|4|1
8|5|1
9|2|2
10|3|1
10|2|2
10|4|3
11|1|4
\.

-- modifyVisit(visitID, custID, dinnerTableID, serverID, numPeople)
COPY ModifyVisit FROM stdin USING DELIMITERS '|';
10|1|2|4|6
12|9|3|2|2
1|2|4|3|8
13|4|7|5|13
6|10|5|1|6
4|6|3|4|7
\.


