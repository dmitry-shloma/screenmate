' Информация о персонажах:
' C:\WINDOWS\system32\oobe\images\qmark.acs (круг с вопросом внутри)
' C:\WINDOWS\srchasst\chars\courtney.acs (бабка на машине)
' C:\WINDOWS\srchasst\chars\earl.acs (уродец на серфе)
' C:\WINDOWS\srchasst\chars\rover.acs (песик как в поиске)
' C:\WINDOWS\msagent\chars\merlin.acs (маг)
' C:\Program Files\Microsoft Office\OFFICE11\CLIPPIT.ACS (скрепка)
' C:\Program Files\Microsoft Office\OFFICE11\DOT.ACS (красный колобок)
' C:\Program Files\Microsoft Office\OFFICE11\F1.ACS (двуногий робот)
' C:\Program Files\Microsoft Office\OFFICE11\LOGO.ACS (логотип офиса)
' C:\Program Files\Microsoft Office\OFFICE11\MNATURE.ACS (земной шар)
' C:\Program Files\Microsoft Office\OFFICE11\OFFCAT.ACS (котенок)
' C:\Program Files\Microsoft Office\OFFICE11\ROCKY.ACS (собака)

' Наименование используемого персонажа Microsoft Agent
strCharacter = "merlin"

Set objMicrosoftAgent = CreateObject("Agent.Control.1")
objMicrosoftAgent.Connected = True
objMicrosoftAgent.Characters.Load strCharacter, strCharacter & ".acs"
Set objCharacter = objMicrosoftAgent.Characters(strCharacter)
With objCharacter
    .Top = 200
    .Left = 600
    .LanguageID = &h409
    .Show
    'Здороваемся с пользователем.
    .Play "Greet"
    .Play "RestPose"
    .Think "Привет!"
    'Завершаем чтение информации
    .Play "ReadReturn"
'    'Прощаемся с пользователем.
'    .Play "Wave"
End With

' Синхронизируем анимацию со скриптом и завершаемся.
Set objCharacterRequest = objCharacter.Hide

Do Until objCharacterRequest.Status = 0 ' Complete = 0
    Wscript.Sleep 100
Loop
Set objCharacter = Nothing

objMicrosoftAgent.Characters.Unload strCharacter