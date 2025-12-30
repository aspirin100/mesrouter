.PHONY: run buildimg clean

PROJ_NAME := mesrouter

buildimg:
	docker build -t $(PROJ_NAME)_img ../

run: 
	docker run --name=$(PROJ_NAME) $(PROJ_NAME)_img

clean:
	docker rm -f $(PROJ_NAME) 2>/dev/null
	docker image rm $(PROJ_NAME)_img