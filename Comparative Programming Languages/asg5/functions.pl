/*
* functions.pl
* 
* Andy Huang (Ahuang44@ucsc.edu)
* Jackson Mccabe (jbmccabe@ucsc.edu)
* 
* Airline reservation system for the Twilight Zone Airlines.
*/

%
not( X ) :- X, !, fail.
not( _ ).

%to radians 
to_rad( degmin(Deg, Min), Rad) :-
    Rad is (Deg + Min/ 60) * pi / 180.

to_hours( Hours, Minutes, Timefloat ) :-
    Timefloat is Hours + Minutes / 60.

to_minutes( Hours, Minutes) :-
    Minutes is Hours * 60.

format_time(Timefloat) :-
    Hours is floor(Timefloat),
    Minutes is  floor( Timefloat * 60 ) mod 60,
    format( '%02d:%02d', [Hours, Minutes]).

% following 2 functions given in /misc/
to_upper( Lower, Upper) :-
   atom_chars( Lower, Lowerlist),
   maplist( lower_upper, Lowerlist, Upperlist),
   atom_chars( Upper, Upperlist).

print_trip( Action, Code, Name, time( Hour, Minute)) :-
   to_upper( Code, Upper_code),
   format( "%-6s  %3s  %-16s  %02d:%02d",
           [Action, Upper_code, Name, Hour, Minute]),
   nl.

%haversine modified from in functions.pl
haversine_radians( Lat1, Lon1, Lat2, Lon2, Distance ) :-
     
    
    Dlat is Lat2 - Lat1,
    Dlon is Lon2 - Lon1,
    A is sin( Dlat / 2 ) ** 2
    + cos( Lat1 ) * cos( Lat2 ) * sin( Dlon / 2 ) ** 2,
    Dist is 2 * atan2( sqrt( A ), sqrt( 1 - A )),
    Distance is Dist * 3961.

% calculates flight time based on haversine_radians' 
% distance divided by 500mph
flight_time( Departure, Arrival, FlightTime ) :-
    airport( Departure, _, Lat1, Lon1 ),
    airport( Arrival, _, Lat2, Lon2 ),
    %conversion to radians
    to_rad(Lat1, RLat1),
    to_rad(Lon1, RLon1), 
    to_rad(Lat2, RLat2),
    to_rad(Lon2, RLon2), 
    haversine_radians( RLat1, RLon1, RLat2, RLon2, Distance),
    FlightTime is Distance / 500.

% calculates the time the plane arrives based on 
% departure time and flight time from flight_time
landing_time(flight(Departure, Arrival, 
    time(DepartureHours, DepartureMinutes )), ETA) :-
    flight_time(Departure, Arrival, FlightTime),
    to_hours(DepartureHours, DepartureMinutes, DepartureTime),
    ETA is DepartureTime + FlightTime.

% converts Arrival time and next departure time into minutes.
% check to see if there is 30 minutes between the transfer.
flight_transfer_req(ArrivalTime, time(NextH,NextM)) :-
    to_hours(NextH, NextM, NextTime),
    to_minutes(ArrivalTime, ArrivalMinutes),
    to_minutes(NextTime, NextMinutes),
    ArrivalMinutes + 29 < NextMinutes.

% checks if the flight will land after 24:00 or not
overnight_check(flight(Departure, Arrival, Time)) :-
    landing_time(flight(Departure, Arrival, Time), ArrivalTime),
    ArrivalTime < 24.

% graph traversal to find connecting flights if they exist 
listpath( Node, End, [flight( Node, Next, Time)| Outlist]) :- 
    not(Node = End),
    flight(Node, Next, Time),
    listpath(Next, End, [flight(Node, Next, Time)],  Outlist).

listpath( Node, Node, _, []).

listpath( Node, End, [flight(PrevDep, PrevArriv, PrevTime)|Tried], 
    [flight(Node, Next, Time)|List]) :-
    flight( Node, Next, Time),
    landing_time( flight( PrevDep, PrevArriv, PrevTime ),
     PrevArrivTime),
    %check if there is 30 min inbetween connecting flights
    flight_transfer_req( PrevArrivTime, Time),
    overnight_check( flight( Node, Next, Time)),

    CombinbedList = append( [flight( PrevDep, PrevArriv, PrevTime )], 
        Tried ),
    not( member( Next, CombinbedList )),
    not( Next = PrevArriv ),
    listpath( Next, End, 
    [flight( Node, Next, Time )|CombinbedList], 
    List ).
% Prints the path in the List
printPath([]).
printPath([flight(Departure,Arrival, 
    time(DepartureHours, DepartureMinutes))|List]) :-
    airport(Departure, DepartureName, _, _),
    airport(Arrival, ArrivalName, _, _),
    to_hours( DepartureHours, DepartureMinutes, DepartTime ),
    landing_time(flight(Departure, Arrival, 
        time(DepartTime, DepartureMinutes)), ArrivalTime),
    to_upper(Departure, UpperDep),
    to_upper(Arrival, UpperArriv),
    format( 'depart   %s   %s', [UpperDep, DepartureName]),
    format_time(DepartTime),
    nl, 
    format( 'arrive   %s   %s', [UpperArriv, ArrivalName]),
    format_time(ArrivalTime),
    nl,
    printPath(List).

% Same city provided in Departure and Arrivals.
fly(Departure, Departure) :- 
    write('Error: Departure and Arrival airports are the same'),
    !, 
    fail.


% the main function to be called
fly( Departure, Arrival) :- 
    listpath(Departure, Arrival, List),
    nl,
    printPath(List),
    true.



% Prints an error if airport cannot be found in database
fly(_, _) :-
    write( 'Error: Airport does not exist in database.' ), 
    nl,
    !, 
    fail.
