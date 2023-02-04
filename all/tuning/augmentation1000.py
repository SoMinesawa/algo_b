import os
import random
import sys
import time

from tqdm import tqdm

dir = sys.argv[1]
os.makedirs(f'data/{dir}', exist_ok=True)
# Number of base stations and queries to generate
num_base_stations = 100
num_queries = 100


def make_name(n1):
    n = n1
    if n1 == 10:
        n = 0
    return str(n)


for pin in tqdm(range(1, 11)):
    for psub in range(1, 11):
        for pdel in range(1, 11):
            s = []
            # Generate base station text data
            for i in range(num_base_stations):
                s_i = ''
                for j in range(5000):
                    s_i += str(random.randint(0, 3))
                s.append(s_i)

            # Generate queries and correct answers
            queries = []
            answers = []
            correct_queries = []

            #print(pin, psub, pdel)
            for i in range(num_queries):
                # Generate correct answer
                correct_answer = random.randint(0, num_base_stations - 1)
                # Change range to random length between 20 and 100
                q_start = random.randint(0, 5000-20-1)
                correct_query = s[correct_answer][q_start:min(q_start +
                                                              random.randint(20, 100), 5000)]
                # print(correct_query)
                answers.append(correct_answer)
                correct_queries.append(correct_query)

                # Generate query based on correct answer
                while True:
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
                                    if j == 0:
                                        query = ch + query[j:]
                                    else:
                                        query = query[:j-1] + ch + query[j:]
                                    break

                    # Add insertion
                    #print('after sub:   ', query)
                    for j in range(len(query)):
                        if random.random()*100 < pin:  # Add insertion
                            query = query[:j] + \
                                str(random.randint(0, 3)) + query[j:]
                            j += 1
                    #print('after insert:', query)

                    if 20 <= len(query) and len(query) <= 100:
                        break
                queries.append(query)

            file_num = ''.join([make_name(i) for i in [pin, psub, pdel]])
            idata_file_name = f'idata_{str(file_num)}'
            answer_file_name = f'answer_{str(file_num)}'

            # Write idata file
            with open(f'data/{dir}/{idata_file_name}', 'w') as f:
                f.write(
                    f"{pin} {psub} {pdel}\n")
                for s_i in s:
                    f.write(f"{s_i}\n")
                for query in queries:
                    f.write(f"{query}\n")

            # Write answer file
            with open(f'data/{dir}/{answer_file_name}', 'w') as f:
                f.write(
                    f"{pin} {psub} {pdel}\n")
                for answer, correct_query in zip(answers, correct_queries):
                    # Retrieve correct answer
                    f.write(f"{answer+1} {correct_query}\n")
