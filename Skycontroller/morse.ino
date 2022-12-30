
// placeholder code, just flash 'HRV' in morse code on the builtin LED

char* letters[] = {
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* text = "HRV";
int dotDelay = 80;


void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);
}

void loop()
{
	flashMorse(text);
}

void flashMorse(char* txt)
{
	delay(dotDelay * 6);
	
	int i = 0;
	do
	{
		char c = txt[i];
		if (c >= 'A' && c <= 'Z')
		{
			flashChar( letters[c - 'A']);
		}
		else if (c >= 'a' && c <= 'z')
		{
			flashChar(letters[c - 'a']);
		}
		else if (c == ' ')
		{
			flashChar(" ");
		}
		i++;
	} while (txt[i] != NULL);
}

void flashChar(char* sequence)
{
	delay(dotDelay * 4);
	
	int i = 0;
	do 
	{
		flashDotOrDash(sequence[i]);		
		i++;
	} while (sequence[i] != NULL);
}


void flashDotOrDash(char dotOrDash) 
{	
	if (dotOrDash == '.')
	{
		digitalWrite(LED_BUILTIN, HIGH);
		delay(dotDelay);
		
		digitalWrite(LED_BUILTIN, LOW); 
		delay(dotDelay * 2);
	}
	else if (dotOrDash == '-')
	{
		digitalWrite(LED_BUILTIN, HIGH);
		delay(dotDelay * 3);
		
		digitalWrite(LED_BUILTIN, LOW); 
		delay(dotDelay * 2);
	}
	else if (dotOrDash == ' ')
	{
		//digitalWrite(LED_BUILTIN, LOW);
		delay(dotDelay * 3);
	}
	
}