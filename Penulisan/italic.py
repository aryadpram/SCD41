import re
import nltk
from nltk.corpus import words
from collections import defaultdict

# Download the word list if not already available
nltk.download('words')

def load_english_words():
    # Load the set of English words from NLTK
    return set(words.words())

def extract_sentences(text):
    # Split the text into sentences based on periods
    return re.split(r'(?<!\w\.\w.)(?<![A-Z][a-z]\.)(?<=\.|\?)\s', text)

def get_context(sentence, word_start, word_end):
    # Extract context around the word
    start = max(word_start - 20, 0)
    end = min(word_end + 20, len(sentence))
    return sentence[start:end]

def italicize_english_words(tex_content, english_words):
    # Pattern to match words (ignoring already italicized words)
    word_pattern = re.compile(r'\b(?!\\textit\{)[a-zA-Z]+\b(?!\})')
    # Pattern to match quoted text (single or double quotes)
    quoted_pattern = re.compile(r'(["\'])(.*?)\1', re.DOTALL)
    
    def is_english_word(word):
        # Check if the word is a recognized English word
        return word.lower() in english_words

    # Extract sentences
    sentences = extract_sentences(tex_content)
    word_sentences = defaultdict(list)

    for sentence in sentences:
        if quoted_pattern.search(sentence):
            continue
        for match in word_pattern.finditer(sentence):
            word = match.group(0)
            if is_english_word(word):
                word_sentences[word.lower()].append((match.start(), match.end(), sentence.strip()))

    if not word_sentences:
        print("No English words found for potential italicization.")
        return tex_content, []

    # Show the list of unique words with context and prompt for confirmation
    print("English words found for italicization:")
    unique_words = list(word_sentences.keys())
    word_counter = len(unique_words)

    selected_words = set()
    while unique_words:
        word = unique_words.pop(0)
        context_snippets = []
        for word_start, word_end, sentence in word_sentences[word]:
            context_snippets.append(get_context(sentence, word_start, word_end))

        # Display the context snippets first
        print(f"\nContexts for word: {word}")
        for i, snippet in enumerate(context_snippets, start=1):
            print(f"  Context {i}: {snippet}")

        # Ask the user to approve or disapprove each group of words
        response = input(f"\nDo you want to italicize the word '{word}'? (y/n): ").strip().lower()
        if response == 'y':
            selected_words.add(word)
        elif response == 'n':
            continue
        else:
            print("Invalid input. Skipping this word.")
            continue

        word_counter -= 1
        print(f"Words left to review: {word_counter}")

    def replace_italic(match):
        word = match.group(0)
        if word.lower() in selected_words:
            return f'\\textit{{{word}}}'
        return word

    processed_parts = []
    for part in quoted_pattern.split(tex_content):
        if quoted_pattern.match(part):
            processed_parts.append(part)
        else:
            processed_parts.append(word_pattern.sub(replace_italic, part))

    return ''.join(processed_parts), selected_words

def main():
    input_file = 'input.tex'
    output_file = 'output.tex'
    
    # Load the English word list
    english_words = load_english_words()

    # Read the content from the input file
    with open(input_file, 'r') as file:
        tex_content = file.read()
    
    # Process the content
    processed_content, selected_words = italicize_english_words(tex_content, english_words)
    
    # Write the processed content to the output file
    if selected_words:
        with open(output_file, 'w') as file:
            file.write(processed_content)
        print(f"\nProcessed content written to {output_file}.")
        print(f"Words italicized: {len(selected_words)}")
        print(f"Italicized words: {', '.join(selected_words)}")
    else:
        print("\nNo changes were made.")

if __name__ == "__main__":
    main()
