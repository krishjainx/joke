obj-m += joke.o
TARGET_SCRIPT = fetch_joke.py
SERVICE = fetch_joke.service
TIMER = fetch_joke.timer
SCRIPT_DEST = /usr/local/bin
SERVICE_DEST = /etc/systemd/system

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install: all
	# Install the kernel module
	sudo cp -f $(TARGET_SCRIPT) $(SCRIPT_DEST)
	sudo chmod +x $(SCRIPT_DEST)/$(TARGET_SCRIPT)
	sudo cp -f $(SERVICE) $(SERVICE_DEST)
	sudo cp -f $(TIMER) $(SERVICE_DEST)
	# Reload systemd to recognize new units
	sudo systemctl daemon-reload
	# Enable and start the timer
	sudo systemctl enable $(TIMER)
	sudo systemctl start $(TIMER)

uninstall:
	# Stop the timer
	sudo systemctl stop $(TIMER)
	sudo systemctl disable $(TIMER)
	# Remove the service and timer files
	sudo rm -f $(SERVICE_DEST)/$(SERVICE)
	sudo rm -f $(SERVICE_DEST)/$(TIMER)
	# Unload the kernel module (if necessary)
	sudo rmmod joke || true
	# Clean up the script
	sudo rm -f $(SCRIPT_DEST)/$(TARGET_SCRIPT)
	# Reload systemd to remove the units from the list
	sudo systemctl daemon-reload

