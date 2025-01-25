import pandas as pd
import json

# Convert CSV data from 'dataset/dataset.csv' file to JSON format and write to file
submission = [
    # Write your JSON formatted data here
    
]

with open("submission.json","w") as file:
    json.dump(submission, file, indent = 4)