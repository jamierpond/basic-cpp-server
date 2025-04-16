

APP_NAME=pond-audio
REPOSITORY_NAME = docker.io/$(APP_NAME)

docker:
	docker build -t $(APP_NAME) .

run:
	docker run -it --rm \
		-v $(shell pwd):/app \
		-v /tmp:/tmp \
		-v /var/run/docker.sock:/var/run/docker.sock \
		--network host \
		$(APP_NAME)



