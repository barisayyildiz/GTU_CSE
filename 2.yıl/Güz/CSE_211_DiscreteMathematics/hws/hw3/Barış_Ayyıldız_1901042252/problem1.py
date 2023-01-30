# Relation class, contains
# -set
# -relations
# -output string
class Relation():
	def __init__(self, Set):
		# set is an array
		self.set = Set
		# relations is a dictionary that contains all the relations
		self.relations = {}
		self.temp = []
		# output message, a string type
		# will be generated in run function
		self.output = ""

	def drawHasseDiagram(self, filename):
		# make a copy of the relations dictionary
		# hasse = deepcopy(self.relations)
		hasse = []
		toBeRemoved = []
		for key in self.relations:
			for value in self.relations[key]:
				hasse.append([key,value])

		self.output = "n\nPOSET :"

		for arr in hasse:
			self.output += " (" + arr[0] + ", " + arr[1] + "),"
		self.output = self.output[:-1]
		self.output += "\n"

		# remove reflexivity
		for key in self.set:
			hasse.remove([key,key])

		# remove transitivity
		for i in hasse:
			for j in hasse:
				if i[1] == j[1] and i != j:

					if [i[0],j[0]] in hasse:
						toBeRemoved.append(i)
					elif [j[0],i[0]] in hasse:
						toBeRemoved.append(j)

		for arr in toBeRemoved:
			if arr in hasse:
				hasse.remove(arr)

		for arr in hasse:
			self.output += arr[0] + "," + arr[1] + "\n"

		# writing to the file
		fout = open(filename, "a")
		fout.write(self.output)
		fout.close()


	def genPoset(self):
		# this function will generate an output string
		# and write it to the file, filename
		
		# writes 'n' to the file
		self.output = "n\n"

		# does all the tests and writes the result to the file
		self.isReflexive()
		for i in self.temp:
			self.append(i[0], i[1])

		self.temp = []

		
		self.isAntiSymmetric()

		for i in self.temp:
			self.relations[i[0]].remove(i[1])

		self.temp = []

		self.isTransitive()

		for i in self.temp:
			self.append(i[0],i[1])

		self.temp = []


	def append(self, key, value):
		# appends a relation to the 'relations' dictionary
		# if the relation is a -> b, then
		# key : a, value : b

		if key not in self.set:
			print("%s is not an element of the set" % (key))
			return

		if value not in self.set:
			print("%s is not an element of the set" % (value))
			return

		if key in self.relations:
			self.relations[key].append(value)
		else:
			self.relations[key] = [value]

	def isReflexive(self):
		# Reflexive: R is reflexive if (a, a) ∈ R, ∀ a ∈ A 

		result = True

		for key in self.set:
			if key not in self.relations:
				# key is an element of the set
				# if there is no relation from that element,
				# which means that element is not a key for the dictionary of relations, the relations is not reflexive

				self.temp.append([key,key])
				result = False
				continue
				# return False

			if key not in self.relations[key]:
				# there is a relation from that element
				# but there is not a relation from that element to that element, which means
				# there is an element named 'a' exists in the set, where (a,a) is not an element of the relation
				
				self.temp.append([key,key])
				result = False
		
		# if the program can reach to this point, the relation is reflexive
		
		return result

	def isSymmetric(self):
		# Symmetric: R is symmetric if (b, a) ∈ R whenever (a, b) ∈ R, ∀ a, b ∈ A

		for key in self.set:
			if key not in self.relations:
				# if the element of the set is not in the dictionary, we can simply skip
				continue

			for value in self.relations[key]:
				# if the relation is (a, a), no need to check
				if key == value:
					continue

				if(self.contains(value, key) == False):
					# if the relation contains (a,b) but does not contain (b,a) it is not symmetric
					return False

		# if the program can reach to this point, the relation is symmetric
		return True

	def isAntiSymmetric(self):
		# Anti-symmetric: R is antisymmetric if ∀ a, b ∈ A, (a, b) ∈ R and (b, a) ∈ R implies that a = b

		result = True

		for key in self.set:
			if key not in self.relations:
				# if the element of the set is not in the dictionary, we can simply skip
				continue

			for value in self.relations[key]:
				if key == value:
					# if the relation is (a, a), no need to check
					continue

				if(self.contains(value, key) == True and ([key,value] not in self.temp)):
					# if the relation contains (a,b) and (b,a) but a != b, the relation is not antisymmetric

					self.temp.append([value, key])

					result = False

		# if the program can reach to this point, the relation is antisymmetric
		return result

	def isTransitive(self):

		result = True

		for key in self.relations:
			if key not in self.relations:
				# if the element of the set is not in the dictionary, we can simply skip
				continue

			for value in self.relations[key]:
				if key == value:
					# if the relation is (a, a)
					# no need to check
					continue

				if value not in self.relations:
					# if the element is (a,b), but there is not a relation starting with 'b', such as (b,c)
					# no need to check
					continue

				for _value in self.relations[value]:
					if value == _value:
						# if the elements (a,b) and (b,b), there is already an element (a,b)
						# no need to check
						continue

					if(self.contains(key, _value) == False):
						# if the elements (a,b) and (b,c) exists but (a,c) is not
						# the relation is not transitive
						self.temp.append([key, _value])
						result = False

		# if the program can reach to this point, the relation is transitive
		return result

	def contains(self, key, value):
		# if there is a an element in the relation (key, value) it returns true
		# otherwise false

		if key in self.relations:
			if value in self.relations[key]:
				return True
		return False


if __name__ == "__main__":
	# opens the input.txt file to read commands
	f = open("input.txt", "r")

	# clear the contents of the outputfile
	outputFileName = "output.txt"
	open(outputFileName, "w").close()

	# reads until it reaches end of the file
	while True:
		n = f.readline()

		# if the current line is equal to '\n' or '\0'
		if len(n) == 0 or len(n) == 1:
			break

		n = int(n)

		# reads the set
		# for the set a,b,c this function returns => ['a', 'b', 'c\n']
		arr = (f.readline()).split(",")

		# removes the '\n' from the last string
		if arr[-1][-1] == '\n':
			arr[-1] = arr[-1][:-1]

		# creates a new Relation object with the set of arr
		r = Relation(arr)

		# reads 'n' relations
		for i in range(n):
			# reads a relations
			# for the relation a,b this function returns => ['a', 'b\n']	
			line = (f.readline()).split(",")

			# removes the '\n' from the last string
			if line[-1][-1] == '\n':
				line[-1] = line[-1][:-1]

			# appends the relation (a,b)
			r.append(line[0], line[1])

		# after appending all the relations, calls the genPoset function
		# and writes the output to the outputFileName
		r.genPoset()
		r.drawHasseDiagram(outputFileName)

	# closes the file
	f.close()
