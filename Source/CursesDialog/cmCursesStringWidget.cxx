#include "cmCursesStringWidget.h"
#include "cmCursesForm.h"

inline int ctrl(int z)
{
    return (z&037);
} 

cmCursesStringWidget::cmCursesStringWidget(int width, int height, 
					   int left, int top) :
  cmCursesWidget(width, height, left, top)
{
  m_InEdit = false;
  m_Type = cmCacheManager::STRING;
  set_field_fore(m_Field,  A_NORMAL);
  set_field_back(m_Field,  A_STANDOUT);
  field_opts_off(m_Field,  O_STATIC);
}


bool cmCursesStringWidget::HandleInput(int& key, FORM* form, WINDOW* w)
{
  // 10 == enter
  if (!m_InEdit && ( key != 10 ) )
    {
    return false;
    }

  char* originalStr=0;

  char debugMessage[128];

  // <Enter> is used to change edit mode (like <Esc> in vi).
  while(1) 
    {
    sprintf(debugMessage, "String widget handling input, key: %d", key);
    cmCursesForm::LogMessage(debugMessage);
    // If resize occured during edit, move out of edit mode
    if (!m_InEdit && ( key != 10 && key != KEY_ENTER ) )
      {
      return false;
      }
    // 10 == enter
    if (key == 10 || key == KEY_ENTER) 
      {
      if (m_InEdit)
	{
	cmCursesForm::LogMessage("String widget leaving edit.");
	m_InEdit = false;
	delete[] originalStr;	
	// trick to force forms to update the field buffer
	form_driver(form, REQ_NEXT_FIELD);
	form_driver(form, REQ_PREV_FIELD);
	return true;
	}
      else
	{
	cmCursesForm::LogMessage("String widget entering edit.");
	m_InEdit = true;
	char* buf = field_buffer(m_Field, 0);
	originalStr = new char[strlen(buf)+1];
	strcpy(originalStr, buf);
	}
      }
    else if ( key == KEY_DOWN || key == ctrl('n') ||
	      key == KEY_UP || key == ctrl('p') ||
	      key == KEY_NPAGE || key == ctrl('d') ||
	      key == KEY_PPAGE || key == ctrl('u'))
      {
      m_InEdit = false;
      delete[] originalStr;	
      // trick to force forms to update the field buffer
      form_driver(form, REQ_NEXT_FIELD);
      form_driver(form, REQ_PREV_FIELD);
      return false;
      }
    // esc
    else if (key == 27)
      {
      if (m_InEdit)
	{
	m_InEdit = false;
	this->SetString(originalStr);
	delete[] originalStr;
	touchwin(w); 
	wrefresh(w); 
	return true;
	}
      }
    else if ( key == KEY_LEFT || key == ctrl('b') )
      {
      form_driver(form, REQ_PREV_CHAR);
      }
    else if ( key == KEY_RIGHT || key == ctrl('f') )
      {
      form_driver(form, REQ_NEXT_CHAR);
      }
    else if ( key == ctrl('k') )
      {
      form_driver(form, REQ_CLR_EOL);
      }
    else if ( key == ctrl('a') )
      {
      form_driver(form, REQ_BEG_FIELD);
      }
    else if ( key == ctrl('e') )
      {
      form_driver(form, REQ_END_FIELD);
      }
    else if ( key == ctrl('d') || key == 127 || 
	      key == KEY_BACKSPACE || key == KEY_DC )
      {
      form_driver(form, REQ_DEL_PREV);
      }
    else
      {
      form_driver(form, key);
      }

    touchwin(w); 
    wrefresh(w); 

    key=getch(); 
    }
}

void cmCursesStringWidget::SetString(const char* value)
{
  this->SetValue(value);
}

const char* cmCursesStringWidget::GetString()
{
  return this->GetValue();
}

const char* cmCursesStringWidget::GetValue()
{
  return field_buffer(m_Field, 0);
}
