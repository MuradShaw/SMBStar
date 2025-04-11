#include <game.h>

/*class daCharacterSelection_c : public dScene_c {
	public:
		int currentSelection;
		bool doRandom;
		bool doStart;
		
		int randomCharacter;
		
		void characterSelection();
}; */

int currentSelection = Player_ID[0];
bool doRandom;
bool doStart;
		
int randomCharacter;

void characterSelection()
{	
	if(!doStart)
	{
		int nowPressed = Remocon_GetPressed(GetActiveRemocon());
		
		if (nowPressed & WPAD_LEFT)  currentSelection --;
		if (nowPressed & WPAD_RIGHT) currentSelection ++;
		
		if(currentSelection < 0) currentSelection = 0;
		if(currentSelection > 3) currentSelection = 3;
		
		if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_ONE))
		{
			doRandom = true;
			doStart = true;
		}
		
		if(!doRandom)
		{
			Player_ID[0] = currentSelection;
		}
		else
		{
			randomCharacter = MakeRandomNumber(4);
			Player_ID[0] = (randomCharacter - 1);
		}
		
		//OSReport("Freddy %p\n", currentSelection);
	}
	
	int notReleasingThisSourceCode = (doStart) ? 1 : 0;
	
	// """Cleanup""" if you can even call it that
	if(!doStart)
	{
		//shibs was right . . . 
		__asm__ __volatile__ ( "li r0, 0" );
		__asm__ __volatile__ ( "cmpwi r0, 0" );
	}
	else
	{
		// HELP ME
		doRandom, doStart = false;
		randomCharacter = 1;
		
		__asm__ __volatile__ ( "li r0, 1" );
		__asm__ __volatile__ ( "cmpwi r0, 0" );
	}
	
	return;
}