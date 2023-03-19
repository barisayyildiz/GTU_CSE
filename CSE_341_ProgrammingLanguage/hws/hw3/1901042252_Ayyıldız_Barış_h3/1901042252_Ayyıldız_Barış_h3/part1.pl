% students
student(1, [cse343], _).
student(2, [cse331, cse341], handicapped).
student(3, [cse343, cse321], _).
student(4, [cse341], handicapped).
student(5, [cse341, cse321], _).
student(6, [cse331, cse343, cse321], _).
student(7, [cse331, cse341, cse343, cse321], _).

% instructors
instructor(james,[cse343],[projector]).
instructor(robert,[cse341],[projector]).
instructor(john,[cse331],[smartBoard]).
instructor(michael,[cse321],[projector]).

% rooms
room(z23, 30, [8,9,10,11,12,13,14,15,16,17], [projector, smartBoard, handicapped]).
room(z06, 15, [8,9,10,11,12,13,14,15,16,17], [projector, smartBoard]).
room(z10, 10, [8,9,10,11,12,13,14,15,16,17], [projector]).

% occupancy
occupancy(z23,[8,9,10,11],cse343).
occupancy(z06,[8,9],cse341).
occupancy(z10,[8,9,10],cse331).
occupancy(z23,[15,16],cse321).

% courses
course(cse343, james, 20, z23, [8,9,10,11], [1,3,6,7], [projector, smartBoard]).
course(cse341, robert, 10, z06, [8,9], [2,4,5,7], [smartBoard]).
course(cse331, john, 15, z10, [8,9,10], [2,6,7], [projector]).
course(cse321, michael, 25, z23, [15,16], [3,5,6,7], [projector, smartBoard]).



% % Queries
% Returns true if there is a conflict between two courses
conflict(CourseId1, CourseId2) :-
	course(CourseId1,_,_,RoomId1,Hours1,_,_),
	course(CourseId2,_,_,RoomId2,Hours2,_,_),
	RoomId1 == RoomId2,
	contains(Hours1, Hours2).

% Returns true if the second list contains all the items of the first list 
subset_of_first(List1, List2) :-
    forall(member(Element,List1), member(Element,List2)).

% Return true if there is a common element between two lists
contains([Head|Tail], LIST) :- (member(Head, LIST) ->
                            true;
                            contains(Tail,LIST)).

% enroll(1,cse331).		=> returns true
% enroll(2,cse331)		=> returns false (There is no handicapped access in room z06)
% enroll(2,X) 				=> returns all the available lectures for the handicapped student '2'
enroll(StudentId, CourseId) :-
	student(StudentId,_,Handicap),
	course(CourseId,_,Cap,RoomId,_,Students,_),
	room(RoomId,_,_,Equipments),
	length(Students,N),
	N < Cap,
	(var(Handicap) ->
			true;
			member(Handicap, Equipments)
		).

% assign(z06, cse331).	=> returns true
% assign(z10, cse331).	=> returns false (Lecturer preferences a smartboard but z10 does not have one)
% assign(X, cse343). 		=> returns all available rooms for lecture 'cse343'
assign(RoomId, CourseId) :-
	course(CourseId, Instructor, CourseCap, _, _, _, Needs),
	room(RoomId, RoomCap, _, Equipments),
	instructor(Instructor, _, Preferences),
	RoomCap >= CourseCap,
	subset_of_first(Needs, Equipments),
	subset_of_first(Preferences, Equipments).

