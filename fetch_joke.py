#!/usr/bin/env python

import requests
import os

# Function to get a Chuck Norris joke
def get_chuck_norris_joke():
    url = "https://api.chucknorris.io/jokes/random"
    response = requests.get(url)
    if response.status_code == 200:
        data = response.json()
        return data['value']
    else:
        print("Failed to retrieve joke")
        return None

# Main function
def main():
    joke = get_chuck_norris_joke()
    if joke:
        print(f"Fetched Joke: {joke}")
        with open("/dev/joke", "w") as joke_dev:
            joke_dev.write(joke)
            print("Joke written to /dev/joke")

if __name__ == "__main__":
    main()

