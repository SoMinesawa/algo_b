import random
import time

# Number of base stations and queries to generate
num_base_stations = 100
num_queries = 100

# Generate base station text data
base_stations = []
for i in range(num_base_stations):
  base_station = ''
  for j in range(5000):
    base_station += str(random.randint(0, 3))
  base_stations.append(base_station)

# Generate queries and correct answers
queries = []
answers = []
for i in range(num_queries):
  # Generate query
  query = ''
  for j in range(random.randint(20, 100)):
    query += str(random.randint(0, 3))
  queries.append(query)

  # Generate correct answer
  correct_answer = random.randint(0, num_base_stations - 1)
  answers.append(correct_answer)

# Generate random error rates
error_rates = [random.randint(1, 10), random.randint(1, 10), random.randint(1, 10)]

# Add noise to queries
for i, query in enumerate(queries):
  # Insertion
  for j in range(error_rates[0]):
    index = random.randint(0, len(query) - 1)
    query = query[:index] + str(random.randint(0, 3)) + query[index:]
  # Substitution
  for j in range(error_rates[1]):
    index = random.randint(0, len(query) - 1)
    query = query[:index] + str(random.randint(0, 3)) + query[index + 1:]
  # Missing
  for j in range(error_rates[2]):
    index = random.randint(0, len(query) - 1)
    query = query[:index] + query[index + 1:]
  queries[i] = query

# Generate unique file name based on current timestamp
timestamp = int(time.time())
idata_file_name = f'idata_{timestamp}'
answer_file_name = f'answer_{timestamp}'

# Write idata file
with open(idata_file_name, 'w') as f:
  f.write(f"{error_rates[0]} {error_rates[1]} {error_rates[2]}\n")
  for base_station in base_stations:
    f.write(f"{base_station}\n")
  for query in queries:
    f.write(f"{query}\n")

# Write answer file
with open(answer_file_name, 'w') as f:
  f.write(f"{error_rates[0]} {error_rates[1]} {error_rates[2]}\n")
  for i, answer in enumerate(answers):
    f.write(f"{answer}\n{base_stations[answer]}\n")
