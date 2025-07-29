#include <iostream>
#include <raylib.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

//Game state constants and other global variables
const int start = 0, subject = 1, difficulty = 2, quiz = 3, results = 4;
int correct = 0, wrong = 0;
int state = start;

//Different topics and their counters
const char* topics[] = {
    "Vocabulary", "Grammar", "Arithmetic", "Algebra", "Calculus", "Trigonometry", "Modern", "Geography", "Animals", "Astronomy", "Concepts", "Syntax", "Spelling", "Ancient", "Medieval", "Science", "Literature" };
int topicCount[17] = { 0 };

struct Question 
{
    string subject;
    string difficulty;
    string topic;
    string questionText;
    string op1, op2, op3, op4;
    string corrOp;
};

//Function to trim the start and end of question statements
void trim(string& str);

//Load questions into a vector from a csv file containing questions
vector<Question> loadQuestionsFromFile();

//Function to make buttons for the GUI
bool makeButton(Rectangle button, Color normal, Color hover, Color click, const char* text, Font font, int fontSize);

//Function to check what topic a question belongs to
void topicCheck(const Question& q);

//Start of the main function
int main() 
{
    InitWindow(1920, 1080, "Quiz Game");
    ToggleFullscreen();
    InitAudioDevice();
    SetTargetFPS(60);

    //Load required audio, font and background
    Texture2D background = LoadTexture("C:\\Users\\Dell\\Downloads\\High_resolution_wallpaper_background_ID_77701423996.png");
    Font font = LoadFont("C:\\Users\\Dell\\Downloads\\raleway\\Raleway-Black.ttf");
    Sound buttonClick = LoadSound("C:\\Users\\Dell\\Downloads\\button-click.wav");
    Sound correctSound = LoadSound("C:\\Users\\Dell\\Downloads\\mixkit-correct-answer-tone-2870.wav");
    Sound wrongSound = LoadSound("C:\\Users\\Dell\\Downloads\\wrong-47985.wav");
    Music music = LoadMusicStream("C:\\Users\\Dell\\Downloads\\flash-quiz-loop-236356.wav");
    PlayMusicStream(music);
    
    vector<Question> questions = loadQuestionsFromFile();
    vector<Question> filteredQ;

    string subj;
    string diff;
    int questionNo = 0;

    state = start;

    while (!WindowShouldClose()) 
    {
        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(background, 0, 0, WHITE);
        DrawRectangle(0, 0, 1920, 1080, Fade(BLACK, 0.6f));

        //Start screen
        if (state == start) {
            DrawTextEx(font, "Quiz Game!", { (1920 - MeasureTextEx(font, "Quiz Game!", 80, 3).x) / 2, 200 }, 80, 3, RAYWHITE);

            Rectangle start = { 760, 600, 400, 100 };
            if (makeButton(start, DARKGRAY, LIGHTGRAY, YELLOW, "Start", font, 40)) {
                PlaySound(buttonClick);
                state = subject;
            }
        } 

        //Section to choose the Subject
        else if (state == subject) {
            DrawTextEx(font, "Choose a Subject", { (1920 - MeasureTextEx(font, "Choose a Subject", 70, 3).x) / 2, 150 }, 70, 3, RAYWHITE);
                
            float width = 350, height = 80, spacing = 40;
            float x = (1920 - (2 * width + spacing)) / 2;
            float y = 300;

            if (makeButton({ x, y, width, height }, DARKGRAY, LIGHTGRAY, BLUE, "Maths", font, 40)) {
                PlaySound(buttonClick);
                subj = "Maths";
                state = difficulty;
            }
            if (makeButton({ x + width + spacing, y, width, height }, DARKGRAY, LIGHTGRAY, RED, "History", font, 40)) {
                PlaySound(buttonClick);
                subj = "History";
                state = difficulty;
            }
            if (makeButton({ x, y + height + spacing, width, height }, DARKGRAY, LIGHTGRAY, ORANGE, "General Knowledge", font, 40)) {
                PlaySound(buttonClick);
                subj = "General Knowledge";
                state = difficulty;
            }
            if (makeButton({ x + width + spacing, y + height + spacing, width, height }, DARKGRAY, LIGHTGRAY, PURPLE, "Programming", font, 40)) {
                PlaySound(buttonClick);
                subj = "Programming";
                state = difficulty;
            }
            if (makeButton({ x + width / 2, y + 2 * (height + spacing), width, height }, DARKGRAY, LIGHTGRAY, ORANGE, "English", font, 40)) {
                PlaySound(buttonClick);
                subj = "English";
                state = difficulty;
            }
        } 

        //Section to choose the Difficulty
        else if (state == difficulty) 
        {
            DrawTextEx(font, "Choose a Difficulty", { (1920 - MeasureTextEx(font, "Choose a Difficulty", 70, 3).x) / 2, 150 }, 70, 3, RAYWHITE);

            float width = 350, height = 80, spacing = 40;
            float x = (1920 - (2 * width + spacing)) / 2;
            float y = 300;

            if (makeButton({ x, y, width, height }, DARKGRAY, LIGHTGRAY, BLUE, "Easy", font, 40)) {
                PlaySound(buttonClick);
                diff = "Easy";
                state = quiz;
            }
            if (makeButton({ x + width + spacing, y, width, height }, DARKGRAY, LIGHTGRAY, RED, "Medium", font, 40)) {
                PlaySound(buttonClick);
                diff = "Medium";
                state = quiz;
            }
            if (makeButton({ x + width / 2, y + height + spacing, width, height }, DARKGRAY, LIGHTGRAY, GREEN, "Hard", font, 40)) {
                PlaySound(buttonClick);
                diff = "Hard";
                state = quiz;
            }
        } 

        //Section of quiz where questions appear
        else if (state == quiz) 
        {
            //Filtering questions by selected subject and difficulty
            filteredQ.clear();
            for (const auto& q : questions) {
                if (q.subject == subj && q.difficulty == diff) {
                    filteredQ.push_back(q);
                }
            }

            if (filteredQ.empty() || questionNo >= filteredQ.size()) {
                DrawTextEx(font, "Quiz Complete!", { (1920 - MeasureTextEx(font, "Quiz Complete!", 60, 3).x) / 2, 500 }, 60, 3, RAYWHITE);

                Rectangle result = { 1020, 600, 400, 100 };
                if (makeButton(result, DARKGRAY, LIGHTGRAY, ORANGE, "Results", font, 40)) {
                    PlaySound(buttonClick);
                    state = results;
                }

                Rectangle again = { 500, 600, 400, 100 };
                if (makeButton(again, DARKGRAY, LIGHTGRAY, ORANGE, "Play Again", font, 40)) {
                    PlaySound(buttonClick);
                    questionNo = 0;
                    state = start;
                }
            }
            else 
            {
                //Displaying current question
                Question& q = filteredQ[questionNo];
                DrawTextEx(font, q.questionText.c_str(), { (1920 - MeasureTextEx(font, q.questionText.c_str(), 40, 3).x) / 2, 100 }, 40, 3, RAYWHITE);

                // Display options as buttons
                float width = 400, height = 80, spacing = 40;
                float x = (1920 - (2 * width + spacing)) / 2;
                float y = 300;

                if (makeButton({ x, y, width, height }, DARKGRAY, LIGHTGRAY, YELLOW, q.op1.c_str(), font, 40)) {
                    
                    if (q.op1 == q.corrOp) {
                        correct++;
                        PlaySound(correctSound);
                    }
                    else {
                        PlaySound(wrongSound);
                        wrong++;
                        topicCheck(q);
                    }
                    questionNo++;
                }
                if (makeButton({ x + width + spacing, y, width, height }, DARKGRAY, LIGHTGRAY, YELLOW, q.op2.c_str(), font, 40)) {
                    if (q.op2 == q.corrOp) {
                        correct++;
                        PlaySound(correctSound);
                    }
                    else {
                        PlaySound(wrongSound);
                        wrong++;
                        topicCheck(q);
                    }
                    questionNo++;
                }
                if (makeButton({ x, y + height + spacing, width, height }, DARKGRAY, LIGHTGRAY, YELLOW, q.op3.c_str(), font, 40)) {
                    if (q.op3 == q.corrOp) {
                        correct++;
                        PlaySound(correctSound);
                    }
                    else {
                        PlaySound(wrongSound);
                        wrong++;
                        topicCheck(q);
                    }
                    questionNo++;
                }
                if (makeButton({ x + width + spacing, y + height + spacing, width, height }, DARKGRAY, LIGHTGRAY, YELLOW, q.op4.c_str(), font, 40)) {
                    if (q.op4 == q.corrOp) {
                        correct++;
                        PlaySound(correctSound);
                    }
                    else {
                        PlaySound(wrongSound);
                        wrong++;
                        topicCheck(q);
                    }
                    questionNo++;
                }
            }
        }

        else if (state == results)
        {
            ClearBackground(BLACK);

            DrawTextEx(font, "Results", { (1920 - MeasureTextEx(font, "Results", 80, 3).x) / 2, 150 }, 80, 3, RAYWHITE);

            string corr = "Correct Answers: " + to_string(correct);
            DrawTextEx(font, corr.c_str(), { (1920 - MeasureTextEx(font, corr.c_str(), 60, 3).x) / 2, 300 }, 60, 3, GREEN);
            string wron = "Wrong Answers: " + to_string(wrong);
            DrawTextEx(font, wron.c_str(), { (1920 - MeasureTextEx(font, wron.c_str(), 60, 3).x) / 2, 400 }, 60, 3, RED);

            DrawTextEx(font, "Result by topics:",
                { (1920 - MeasureTextEx(font, "Result by topics:", 50, 3).x) / 2, 550 }, 50, 3, ORANGE);

            int y = 620;
            int mistake = 0;
            string hardest; //Hardest topic which needs to be focused on
            string currentSub;

            //Loop to display topics in which mistakes were made
            for (int i = 0; i < 17; i++) {
                if (topicCount[i] > 0) {
                    string focus = string(topics[i]) + " (" + to_string(topicCount[i]) + " mistakes)";
                    DrawTextEx(font, focus.c_str(), { (1920 - MeasureTextEx(font, focus.c_str(), 40, 3).x) / 2, (float)y }, 40, 3, RAYWHITE);
                    y += 50;

                    if (topicCount[i] > mistake) {
                        mistake = topicCount[i];
                        hardest = topics[i];
                        currentSub = subj;
                    }
                }
            }

            if (!hardest.empty()) {
                string str = "Focus on " + hardest + " in " + currentSub +
                    " (" + to_string(mistake) + " mistakes)";
                DrawTextEx(font, str.c_str(), { (1920 - MeasureTextEx(font, str.c_str(), 50, 3).x) / 2, (float)y + 30 }, 50, 3, ORANGE);
            }

            //Play Again button
            Rectangle again = { 760, 900, 400, 100 };
            if (makeButton(again, DARKGRAY, LIGHTGRAY, ORANGE, "Play Again", font, 40)) {
                PlaySound(buttonClick);
                questionNo = 0;
                correct = 0;
                wrong = 0;

                for (int i = 0; i < 17; i++) {
                    topicCount[i] = 0;
                }
                state = start; 
            }
            }




        EndDrawing();
    }

    //Unload background, font, sounds and music
    UnloadTexture(background);
    UnloadFont(font);
    UnloadSound(buttonClick);
    UnloadSound(correctSound);
    UnloadSound(wrongSound);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}

//Function to trim the start and end of question statements
void trim(string& str)
{
    str.erase(str.begin(), find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !isspace(ch);
        }));
    str.erase(find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !isspace(ch);
        }).base(), str.end());
}

//Load questions into a vector from a csv file containing questions
vector<Question> loadQuestionsFromFile()
{
    ifstream file("quiz_questions.csv");
    vector<Question> questions;
    string line;

    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string subject, difficulty, topic, questionText, option1, option2, option3, option4, correctOption;

        getline(ss, subject, ',');
        getline(ss, difficulty, ',');
        getline(ss, topic, ',');
        getline(ss, questionText, ',');
        getline(ss, option1, ',');
        getline(ss, option2, ',');
        getline(ss, option3, ',');
        getline(ss, option4, ',');
        getline(ss, correctOption, ',');

        trim(subject);
        trim(difficulty);
        trim(topic);
        trim(questionText);
        trim(option1);
        trim(option2);
        trim(option3);
        trim(option4);
        trim(correctOption);

        questions.push_back({ subject, difficulty, topic, questionText, option1, option2, option3, option4, correctOption });

    }

    file.close();
    return questions;
}

//Function to make buttons for the GUI
bool makeButton(Rectangle button, Color normal, Color hover, Color click, const char* text, Font font, int fontSize)
{
    Vector2 mouse = GetMousePosition();
    Color current = normal;

    Vector2 textSize = MeasureTextEx(font, text, fontSize, 2);

    while (textSize.x > button.width - 20)
    {
        fontSize--;
        textSize = MeasureTextEx(font, text, fontSize, 2);
    }

    if (CheckCollisionPointRec(mouse, button))
    {
        current = hover;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            current = click;
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            return true;
        }
    }

    DrawRectangleRec(button, current);

    Vector2 textPosition = {
        button.x + (button.width - textSize.x) / 2,
        button.y + (button.height - textSize.y) / 2
    };

    DrawTextEx(font, text, textPosition, fontSize, 2, WHITE);
    return false;
}

//Function to see answers in which topic were incorrect
void topicCheck(const Question& q) {
    for (int i = 0; i < 17; i++) {
        if (q.topic == topics[i]) {
            topicCount[i]++;
            break;
        }
    }
}

