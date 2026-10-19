#include <iostream>
#include <cstring>
#include <windows.h>

using namespace std;

/*
==========================================================================================================================================
                                                            ДОМАШНЄ ЗАВДАННЯ 24.
                                    Конструктор копіювання, константні методи, explicit. Клас Playlist
==========================================================================================================================================
*/

class Playlist
{
private:
    char* name;
    int* durations;
    int count;

public:
    explicit Playlist(const char* name) : name(nullptr), durations(nullptr), count(0)
    {
        setName(name);
        cout << "  [+] " << this->name << endl;
    }

    //  4. Що станеться, якщо в конструкторі копіювання замість виділення нової пам'яті написати  durations = other.durations; ?
    //  Коли саме програма «зламається» ?
    //  7. (Рівень 2) Чому конструктор копіювання не роблять explicit ? Назвіть рядок вашого main, який тоді перестав би компілюватися.
    Playlist(const Playlist& other) : name(nullptr), durations(nullptr), count(other.getCount())
    {
        setName(other.getName());
        if (count > 0)
        {
            durations = new int[count];
            for (int i = 0; i < count; i++)
            {
                durations[i] = other.getDuration(i);
            }
        }
        cout << "  [=] " << name << endl;
    }

    ~Playlist()
    {
        cout << "  [-] " << name << endl;
        delete[] name;
        delete[] durations;
    }

    void setName(const char* name)
    {
        delete[] this->name;
        size_t length = strlen(name) + 1;
        this->name = new char[length];
        strcpy_s(this->name, length, name);
    }

    void add(int seconds)           // 1. Чому getTotalSeconds можна зробити константним, а add — ні ?
    {
        if (seconds <= 0)
        {
            return;
        }

        int* newDurations = new int[count + 1];
        for (int i = 0; i < count; i++)
        {
            newDurations[i] = durations[i];
        }
        newDurations[count] = seconds;

        delete[] durations;
        durations = newDurations;
        count++;
    }

    // 3. Чому getName повертає const char*, а не char*? Що можна було б зробити з константним плейлистом, якби повертався char*?
    const char* getName() const
    {
        return name;
    }

    int getCount() const
    {
        return count;
    }

    int getDuration(int index) const
    {
        if (index < 0 || index >= count)
        {
            return 0;
        }
        return durations[index];
    }

    int getTotalSeconds() const         // 1. Чому getTotalSeconds можна зробити константним, а add — ні ?
    {
        int total = 0;
        for (int i = 0; i < count; i++)
        {
            total += durations[i];
        }
        return total;
    }

    int getLongest() const
    {
        int longest = 0;
        for (int i = 0; i < count; i++)
        {
            if (durations[i] > longest)
            {
                longest = durations[i];
            }
        }
        return longest;
    }

    void print() const;          // 2. Який тип має покажчик this всередині print? Що через це НЕ можна робити в print?
};

// Допоміжна функція: секунди у форматі хв:сс
void printTime(int seconds)
{
    cout << seconds / 60 << ":";
    if (seconds % 60 < 10)
    {
        cout << "0";
    }
    cout << seconds % 60;
}

void Playlist::print() const      // 2. Який тип має покажчик this всередині print? Що через це НЕ можна робити в print?
{
    cout << "  «" << name << "», пісень: " << count << endl;
    for (int i = 0; i < count; i++)
    {
        cout << "    " << i + 1 << ". ";
        printTime(durations[i]);
        cout << endl;
    }
}

//  5. (Рівень 2) Що ви побачили у виводі при виклику printCopy(morning) ? Чому пісня на 60 секунд не з'явилася в morning?
//  Чому printSummary приймає const Playlist&, а не Playlist ?
void printSummary(const Playlist& playlist)
{
    cout << "  Підсумок «" << playlist.getName() << "»: пісень " << playlist.getCount() << ", ";
    printTime(playlist.getTotalSeconds());
    cout << ", найдовша ";
    printTime(playlist.getLongest());
    cout << endl;
}

//  5. (Рівень 2) Що ви побачили у виводі при виклику printCopy(morning) ? Чому пісня на 60 секунд не з'явилася в morning?
//  Чому printSummary приймає const Playlist&, а не Playlist ?
void printCopy(Playlist playlist)
{
    playlist.add(60);           // змінюється лише копія
    playlist.print();
}   // тут знищується копія: [-]

// Повертає посилання на довший плейлист — без копіювання
const Playlist& longer(const Playlist& first, const Playlist& second)
{
    if (first.getTotalSeconds() >= second.getTotalSeconds())
    {
        return first;
    }
    return second;
}

// Новий плейлист: копія first + усі пісні second. Повертається ЗА ЗНАЧЕННЯМ
Playlist combine(const Playlist& first, const Playlist& second)
{
    Playlist result = first;    // конструктор копіювання
    result.setName("Мікс");
    for (int i = 0; i < second.getCount(); i++)
    {
        result.add(second.getDuration(i));
    }
    return result;
}

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "========== РІВЕНЬ 1 ==========" << endl;
    Playlist morning("Ранкова");
    morning.add(215);
    morning.add(184);
    morning.add(302);
    morning.add(-5);
    morning.print();

    const Playlist favorite = morning;
    favorite.print();
    cout << "  Загальна тривалість: ";
    printTime(favorite.getTotalSeconds());
    cout << endl;

    printSummary(morning);
    printSummary(favorite);

    cout << endl << "========== РІВЕНЬ 2 ==========" << endl;

    cout << "--- 1. Копії ---" << endl;
    Playlist workout = morning;
    Playlist backup(morning);
    workout.setName("Тренування");
    workout.add(240);
    morning.print();
    backup.print();
    workout.print();

    cout << "--- 2. Передача за значенням ---" << endl;
    //  5. (Рівень 2) Що ви побачили у виводі при виклику printCopy(morning) ? Чому пісня на 60 секунд не з'явилася в morning?
    //  Чому printSummary приймає const Playlist&, а не Playlist ?
    printCopy(morning);
    morning.print();

    cout << "--- 3. explicit ---" << endl;
    //  6. (Рівень 2) Що відбувалося в п. 3 (викликати printSummary("Вечірня") БЕЗ explicit)?
    //  Які конструктори і деструктори викликалися і коли?
    printSummary(Playlist("Вечірня"));

    cout << "--- 4. Довший плейлист ---" << endl;
    printSummary(longer(morning, workout));

    cout << endl << "========== РІВЕНЬ 3 ==========" << endl;

    // Об'єднання: результат — новий незалежний плейлист
    Playlist mix = combine(morning, workout);
    mix.print();
    printSummary(morning);
    printSummary(workout);

    cout << endl << "Кінець main" << endl;
    return 0;
}   // деструктори mix, backup, workout, favorite, morning — у зворотному порядку

/*
    
  1. Чому getTotalSeconds можна зробити константним, а add — ні?
  2. Який тип має покажчик this всередині print? Що через це НЕ можна робити в print?
  3. Чому getName повертає const char*, а не char*? Що можна було б зробити з константним плейлистом, якби повертався char*?
  4. Що станеться, якщо в конструкторі копіювання замість виділення нової пам'яті написати  durations = other.durations; ?
     Коли саме програма «зламається»?
  5. (Рівень 2) Що ви побачили у виводі при виклику printCopy(morning)? Чому пісня на 60 секунд не з'явилася в morning?
     Чому printSummary приймає const Playlist&, а не Playlist?
  6. (Рівень 2) Що відбувалося в п. 3 (викликати printSummary("Вечірня") БЕЗ explicit)? Які конструктори і деструктори викликалися і коли?
  7. (Рівень 2) Чому конструктор копіювання не роблять explicit? Назвіть рядок вашого main, який тоді перестав би компілюватися.
 
  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ»

  1. 

  2. 

  3.

  4. 

  5. 

  6. 

  7. 
*/
