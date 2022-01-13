# Automatisches Starten des Ebus Daemons auf dem Raspberry Pi

Die Datei `ebusd.service` muss zunächst in unter `/etc/systemd/system` abgelegt werden.

Um den Service zu aktivieren:
`sudo systemctl enable ebusd.service`

Um den Service zu starten:
`sudo systemctl start ebusd.service`

Um Log-Meldungen des Service anzusehen:
`journalctl -u ebusd.service`

Um Log-Meldungen des Service zu löschen:
`sudo journalctl --rotate`
`sudo journalctl --vacuum-time=1s`
