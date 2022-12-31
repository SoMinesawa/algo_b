import os
import random
import time

from tqdm import tqdm

# Number of base stations and queries to generate
num_base_stations = 100
num_queries = 100


for file_num in tqdm(range(111, 1000)):
    # Generate base station text data
    s = []
    for i in range(num_base_stations):
        s_i = ''
        for j in range(5000):
            s_i += str(random.randint(0, 3))
        s.append(s_i)

    # Generate queries and correct answers
    queries = []
    answers = []
    correct_queries = []

    # Generate error rates based on file number
    error_rates = [(file_num // 100), (file_num %
                                       100) // 10, file_num % 10]
    pin = error_rates[0]
    psub = error_rates[1]
    pdel = error_rates[2]

    #print(pin, psub, pdel)
    for i in range(num_queries):
        # Generate correct answer
        correct_answer = random.randint(0, num_base_stations - 1)
        # Change range to random length between 20 and 100
        q_start = random.randint(0, 100-20-1)
        correct_query = s[correct_answer][q_start:min(q_start +
                                          random.randint(20, 100), 100)]
        answers.append(correct_answer)
        correct_queries.append(correct_query)

        # Generate query based on correct answer
        query = ''
        #print("correct_query", correct_query)
        # Add missing
        for j in range(len(correct_query)):
            if random.random()*100 < pdel:
                continue
            else:  # No error
                query += correct_query[j]
        #print('after miss:  ', query)

        # Add substitution
        for j in range(len(query)):
            if random.random()*100 < psub:
                while True:
                    ch = str(random.randint(0, 3))
                    if ch != query[j]:
                        if j == len(query):
                            query = query[:j] + ch + query[j+1:]
                        else:
                            query = query[:j] + ch + query[j+1:]
                        break

        # Add insertion
        #print('after sub:   ', query)
        for j in range(len(query)):
            if random.random()*100 < pin:  # Add insertion
                query = query[:j] + str(random.randint(0, 3)) + query[j:]
        #print('after insert:', query)

        queries.append(query)

    # Add noise to queries
    for i in len(queries):

        while True:
            query = queries[i]
            # Insertion
            if error_rates[0] > 0 and len(query) > 0:
                for j in range(error_rates[0]):
                    if len(query) > 0:  # Add check to make sure query is not empty
                        index = random.randint(0, len(query) - 1)
                        query = query[:index] + \
                            str(random.randint(0, 3)) + query[index:]
            # Substitution
            for j in range(error_rates[1]):
                if len(query) > 0:  # Add check to make sure query is not empty
                    index = random.randint(0, len(query) - 1)
                    query = query[:index] + \
                        str(random.randint(0, 3)) + query[index + 1:]
            # Missing
            if error_rates[2] > 0 and len(query) > 0:
                for j in range(error_rates[2]):
                    if len(query) > 0:  # Add check to make sure query is not empty
                        index = random.randint(0, len(query) - 1)
                        query = query[:index] + query[index + 1:]

            if 20 <= len(query) and len(query) <= 100:
                break

        queries[i] = query

    # Generate unique file name based on current timestamp
    timestamp = int(time.time())
    idata_file_name = f'idata_{str(file_num)}'
    answer_file_name = f'answer_{str(file_num)}'

    # Write idata file
    with open(f'data/{idata_file_name}', 'w') as f:
        f.write(f"{error_rates[0]} {error_rates[1]} {error_rates[2]}\n")
        for s_i in s:
            f.write(f"{s_i}\n")
        for query in queries:
            f.write(f"{query}\n")

    # Write answer file
    with open(f'data/{answer_file_name}', 'w') as f:
        f.write(f"{error_rates[0]} {error_rates[1]} {error_rates[2]}\n")
        for i, query in enumerate(queries):
            # Retrieve correct answer
            f.write(f"{i + 1} {correct_queries[i]}\n")
