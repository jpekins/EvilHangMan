#include <iostream>
#include <iomanip>
#include <fstream> // For ifstream
#include <string> // For string, getline
#include <sstream> // For stringstream
#include <cctype> // For alnum
#include <set>
#include <set>
#include <multimap.h>
#include <algorithm>
#include <numeric>
#include <stack>
#include <queue>
#include <iterator>
using namespace std;

int calculateOnShortenedWord(char* shortenedWord, int letPos, set<string>::iterator *itr, string guess, int wordLengthChosen);
bool gamePlay(int wordLengthChosen, deque<string> &dequeofWords, set<string> &lettersGuessedWrong, map<int, string> &lettersGuessedRight);
char* getSubstring(int letPos, int wordLengthChosen);
string nextGuess(set<string> lettersGuessedWrong, map<int, string> lettersGuessedRight);
bool alreadyGuessed(string guess, set<string> lettersGuessedWrong, map<int, string> lettersGuessedRight);
map<int, string> constructMap(int numberGuesses);
bool yorNbool();
int getNumberGuesses();
deque<string> dequeofwordsLengthX(int wordLengthChosen);
string GetLine();
int GetReal();
int StringtoInt(string myString);
int getWordLength();
void PrintSingleInt(int value);


bool gamePlay(int wordLengthChosen, deque<string> &dequeofWords, set<string> &lettersGuessedWrong, map<int, string> &lettersGuessedRight)
{
	bool keepTurn = false;
	cout << "What is your next guess?" << endl;
	string guess = nextGuess(lettersGuessedWrong, lettersGuessedRight);
	deque<string>::iterator itr = dequeofWords.begin();
	while(itr != dequeofWords.end())//first check to see if the letter is within our remaining list
	{
		if ((*itr).find(guess) != string::npos) keepTurn = true;
		++itr;
	}
	if (keepTurn)
	{
		set<int> setTool;
		string tmpString;
		int found;
		multimap<set<int>, string> collection;
		itr = dequeofWords.begin();
		while(itr != dequeofWords.end())//get rid of the ones that didn't have our letter
		{
			tmpString = *itr;
			if (tmpString.find_first_of(guess) == string::npos) 
			{
				itr = dequeofWords.erase(itr);
			} else//for those that do replace the letter with a "*", then add the location to a set
			{//then add the set and the modified string to a multimap as a pair
				setTool.clear();
				found = tmpString.find_first_of(guess);
				while(found != string::npos) {
					tmpString[found] = '*';
					setTool.insert(found);
					found = tmpString.find_first_of(guess, found + 1);
				}
				collection.insert( pair<set<int>, string>(setTool, tmpString) );
				++itr;
			}
		}
		itr = dequeofWords.begin();
		set<int> winningset;
		vector<multimap<set<int>, string> > winnerStorage;
		stack<multimap<set<int>, string> > loserStorage;
		int runningCountforThisGroup, counter;
		int winnerTotal = 0;
		int oneMoreforHumanunderstanding;
		multimap<set<int>, string> mMapTool;
		multimap<set<int>, string>::iterator mmapItr = mMapTool.begin();
		set<int> compareset;
		for (multimap<set<int>, string>::iterator itr2 = collection.begin(); itr2 != collection.end(); ++itr2) 
		{//classify by letter location(s)
			compareset = itr2->first;
			pair<multimap<set<int>, string>::iterator, multimap<set<int>, string>::iterator> myPair = collection.equal_range(compareset);
			runningCountforThisGroup = 0;
			for (multimap<set<int>, string>::iterator itr3 = myPair.first; itr3 != myPair.second; ++itr3)
			{
				mMapTool.clear();
				mMapTool.insert(*itr3);
				loserStorage.push(mMapTool);
				++runningCountforThisGroup;
			}
			if (runningCountforThisGroup > winnerTotal) {//we have a winner
				while (!winnerStorage.empty()) {
					winnerStorage.pop_back();
				}
				mmapItr = mMapTool.begin();
				winningset = mmapItr->first;
				counter = 0;
				winnerTotal = runningCountforThisGroup;
				while (!loserStorage.empty()) {//put in winnerStorage for later use
					winnerStorage.push_back(loserStorage.top());
					loserStorage.pop();
				} 
			} while (!loserStorage.empty())//this one didn't win
			{
				loserStorage.pop();
			}
		} 
		cout << "You have correctly guessed that there is the letter \"" << guess[0] << "\" in the ";
		for (set<int>::iterator winsetItr = winningset.begin(); winsetItr != winningset.end(); ++winsetItr) 
		{//add one to facilitate non-programmer understanding
			oneMoreforHumanunderstanding = *winsetItr + 1;
			cout << oneMoreforHumanunderstanding << " ";
		}
		(counter > 1) ? cout << " letter slots " : cout << "letter slot ";
		cout << "of the word." << endl;
		int temp;
		for(set<int>::iterator winsetItr = winningset.begin(); winsetItr != winningset.end(); ++winsetItr)
		{//insert into lettersguessed right multimap so the letters correctly guessed are shown to user
			temp = *winsetItr;
			pair<map<int, string>::iterator, bool> result = lettersGuessedRight.insert(make_pair(temp, guess));
			if(!result.second) result.first->second = guess;
		}
		dequeofWords.clear();//clear out deque of words, only add in from the family that was the most popular from our last search
		for (vector<multimap<set<int>, string> >::reverse_iterator itr4 = winnerStorage.rbegin(); itr4 != winnerStorage.rend(); ++itr4)
		{//putting the letters back on
			mMapTool = *itr4;
			mmapItr = mMapTool.begin();
			tmpString = mmapItr->second;
			int found = tmpString.find_first_of('*');
			while(found != string::npos) {
				(tmpString)[found] = guess[0];
				found = tmpString.find_first_of('*',found + 1);
			}
			dequeofWords.push_back(tmpString);
			tmpString.clear();
		}
		return keepTurn;
	}
	else//letter guessed missed all of the current words
	{
		lettersGuessedWrong.insert(guess); return keepTurn;
	}
}

void PrintSingleInt(int value)
{
	cout << value << endl;
}


string nextGuess(set<string> lettersGuessedWrong, map<int, string> lettersGuessedRight) 
{
	string result;
	result = GetLine();
	while(!isalpha(result[0]) || alreadyGuessed(result, lettersGuessedWrong, lettersGuessedRight) \
		  || result.length() != 1)
	{
		cout << "Pick a single letter not chosen before!" << endl;
		result = GetLine();
	}
	return result;
}

bool alreadyGuessed(string guess, set<string> lettersGuessedWrong, map<int, string> lettersGuessedRight)
{
	string temp;
	for (set<string>::iterator wrongItr = lettersGuessedWrong.begin(); wrongItr != lettersGuessedWrong.end(); ++wrongItr)
	{
		temp = *wrongItr;
		if (temp.compare(guess) == 0) return true;
	}
	for (map<int, string>::iterator rightItr = lettersGuessedRight.begin(); rightItr != lettersGuessedRight.end(); ++rightItr)
	{
		temp = rightItr->second;
		if (temp.compare(guess) == 0) return true;
	}
	return false;
}

map<int, string> constructMap(int numberGuesses) 
{
	map<int, string> lettersGuessedRight;
	for(int i = 0; i < numberGuesses; ++i)
	{
		lettersGuessedRight.insert(make_pair(i, "-"));
	}
	return lettersGuessedRight;
}

bool yorNbool()
{
	stringstream converter;
	converter << GetLine();
	string result;
	converter >> result;
	while(converter.fail() ||(  result != "yes" && result != "no"))
	{
		cout << "'yes' or 'no'!" << endl;
		converter.clear();
		converter << GetLine();
		converter >> result;
	} if (result == "yes") return true;
	else return false;
}

int getNumberGuesses() 
{
	cout << "How many chances do you want to guess?" << endl;
	int answer = GetReal();
	/* If we failed to read in an integer, we need to report an error. */
	while( answer <= 1 || answer >= 26 || isalpha(answer))
	{
		cout << "Pick a real number length (above 1)!" << endl;
		answer = GetReal();
	}
	return answer;
}



/* 
 * A function to open a file, but only read in words of a length chosen by user. Inspired by Lecture code 1.1.
 */
deque<string> dequeofWordsLengthX(int wordLengthChosen) 
{
	deque<string> myData;
	deque<string>::iterator dequeItr = myData.begin();
	ifstream myStream("dictionary.txt");
	if(myStream.fail())
	{
		// Report an error to cerr, the error-logging stream, then quit with a
		// nonzero result to signal an error to the operating system.
		cerr << "Cannot open file!" << endl;
	}
	string word;
	while(true)
	{	
		myStream >> word;
		if (word.length() == wordLengthChosen)
		{	
			myData.push_back(word);
		} 
		if(myStream.fail())
		{
			if(myStream.eof())	break;
		}
		word.clear();
	}
	myStream.close();
	return myData;
}

int getWordLength() 
{
	cout << "Welcome to REGULAR hangman! How long would you like your word to be this time?" << endl;
	int answer = GetReal();
	while( answer <= 1 || answer >= 20 || isalpha(answer))
	{
		cout << "Pick a real number length (above 1 and under 20)!" << endl;
		answer = GetReal();
	}
	return answer;
}

/* Read an integer, prompting until the user enters valid data. Taken from Lecture code 1.0 */
int GetReal()
{
	while(true)
	{
		/* Fill a stringstream with data read from GetLine().  This means that the user cannot
		 * gum up the input with extraneous data or put cin into a fail state.
		 */
		stringstream converter;
		converter << GetLine();
		/* Try to read an int out of the stream. */
		int result;
		converter >> result;
		/* If we failed to read in an integer, we need to report an error. */
		if(converter.fail())
			cout << "Please enter an integer." << endl;
		else
		{
			/* Otherwise, we know that we've successfully extracted an integer, but there might
			 * be some extra data lying around in the string that could be problematic.  To
			 * check if this is the case, we'll try to read a single character from the stream.
			 * If this succeeds, there's extra data lying around and we can report an error.
			 * Otherwise, if this fails, we know that the input was an int and only an int and
			 * we can report the result to the caller.
			 */
			char remaining;
			converter >> remaining;
			
			/* If the stream broke, we ran out of characters and are done. */
			if(converter.fail())
				return result;
			else
				cout << "Unexpected character: " << remaining << endl;
		}
		
		cout << "Retry: ";
	}
}

/* Reads a full line of text from the user, then returns it. Used from Lecture code 0.2 */
string GetLine()
{
	string result;
	getline(cin, result); // Reads a full line from cin, stores in result.
	return result;
}

int main() 
{	
	while (true)
	{
		int wordLengthChosen = getWordLength();
		deque<string> dequeofWords(10000);
		dequeofWords = dequeofWordsLengthX(wordLengthChosen);
		string wordIfGameOver;
		int numberGuesses = getNumberGuesses();
		cout << "Do you want to have a running total of the number of words remaining in the word list? yes or no? " << endl;
		bool wordsRemainingDesired = yorNbool();
		bool gameWon = true;
		bool currentGame = true;
		string tempString;
		int found;
		set<string> lettersGuessedWrong;
		map<int, string> lettersGuessedRight = constructMap(wordLengthChosen);
		map<int, string>:: iterator lgrItr = lettersGuessedRight.begin();
		while (currentGame)
		{
			cout << "You have " << numberGuesses << " guesses left." << endl;
			if (!lettersGuessedWrong.empty())
			{
				cout << "You have guessed these letters incorrectly: ";
				for(set<string>::iterator itr = lettersGuessedWrong.begin(); itr != lettersGuessedWrong.end(); ++itr)
				{
					cout << *itr;
				}
				cout << endl;
			}
			cout << "Here is your progress so far: ";
			for(map<int, string>::iterator itr = lettersGuessedRight.begin(); itr != lettersGuessedRight.end(); ++itr)
			{
				cout << itr->second <<  " ";
			}
			cout << endl;
			if (wordsRemainingDesired) cout << "You have " << dequeofWords.size() << " possible words remaining." << endl;
			bool winTurn = gamePlay(wordLengthChosen, dequeofWords, lettersGuessedWrong, lettersGuessedRight);
			gameWon = true;
			wordIfGameOver = dequeofWords.front();
			for (lgrItr = lettersGuessedRight.begin(); lgrItr != lettersGuessedRight.end(); ++lgrItr) {
				tempString = lgrItr->second;
				found = tempString.find('-');
				if (found != string::npos) gameWon = false;
			}
			if (!winTurn) 
			{
				numberGuesses--; cout << "You're guess was wrong!" << endl;
			}
			if (numberGuesses == 0)
			{
				cout << "You Lost! Your word was \"" << wordIfGameOver + "\"." << endl;
				cout << "Would you like to play again?" << endl;
				bool playAgain = yorNbool();
				if (!playAgain) {
					return 0;
				}
				currentGame = false;
			}if (gameWon)
			{
				cout << "You Won! You correctly guessed the word " << wordIfGameOver << ". Congratulations!" << endl;
				cout << "Would you like to play again?" << endl;
				bool playAgain = yorNbool();
				if (!playAgain) {
					return 0;
				}
				currentGame = false;
			}
		}
	}
}