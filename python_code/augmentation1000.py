import os
import random
import time
from tqdm import tqdm

# Number of base stations and queries to generate
num_base_stations = 100
num_queries = 100

# Create folder to store generated files
folder_name = f'generated_files_{int(time.time())}'
os.mkdir(folder_name)

for file_num in tqdm(range(1000)):
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
    # Generate correct answer
    correct_answer = random.randint(0, num_base_stations - 1)
    correct_query = base_stations[correct_answer][:random.randint(20, 100)]  # Change range to random length between 20 and 100
    answers.append(correct_answer)

    # Generate error rates based on file number
    error_rates = [file_num // 100, file_num % 100 // 10, file_num % 10]

    # Generate query based on correct answer
    query = ''
    for j in range(len(correct_query)):
      # Calculate probability of each type of error
      pin = error_rates[0] / 10
      psub = error_rates[1] / 10
      pdel = error_rates[2] / 10
      # Add error with probability equal to error_rate
      if random.random() < pin:  # Add insertion
        query += str(random.randint(0, 3))
      elif random.random() < psub:  # Add substitution
        query += str(random.randint(0, 3))
      elif random.random() < pdel:  # Add missing
        continue
      else:  # No error
        query += correct_query[j]
    queries.append(query)

  # Add noise to queries
  for i, query in enumerate(queries):
    # Insertion
    if error_rates[0] > 0 and len(query) > 0:
      for j in range(error_rates[0]):
        if len(query) > 0:  # Add check to make sure query is not empty
          index = random.randint(0, len(query) - 1)
          query = query[:index] + str(random.randint(0, 3)) + query[index:]
    # Substitution
    for j in range(error_rates[1]):
      if len(query) > 0:  # Add check to make sure query is not empty
        index = random.randint(0, len(query) - 1)
        query = query[:index] + str(random.randint(0, 3)) + query[index + 1:]
    # Missing
    if error_rates[2] > 0 and len(query) > 0:
      for j in range(error_rates[2]):
        if len(query) > 0:  # Add check to make sure query is not empty
          index = random.randint(0, len(query) - 1)
          query = query[:index] + query[index + 1:]
    queries[i] = query


  # Generate unique file name based on current timestamp
  timestamp = int(time.time())
  idata_file_name = f'idata_{timestamp}_{file_num}'
  answer_file_name = f'answer_{timestamp}_{file_num}'

  # Write idata file
  with open(f'{folder_name}/{idata_file_name}', 'w') as f:
      f.write(f"{error_rates[0]} {error_rates[1]} {error_rates[2]}\n")
      for base_station in base_stations:
          f.write(f"{base_station}\n")
      for query in queries:
          f.write(f"{query}\n")

  # Write answer file
  with open(f'{folder_name}/{answer_file_name}', 'w') as f:
      f.write(f"{error_rates[0]} {error_rates[1]} {error_rates[2]}\n")
      for i, query in enumerate(queries):
          # Retrieve correct answer
          correct_query = base_stations[answers[i]][:len(query)]
          f.write(f"{i + 1} {correct_query}\n")
