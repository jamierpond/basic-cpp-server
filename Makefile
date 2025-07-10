
APP_NAME=pond-audio
REPOSITORY_NAME = docker.io/$(APP_NAME)

docker:
	docker build --progress=plain -t $(APP_NAME) .

run:
	docker run -it --rm \
		-v $(shell pwd):/app \
		-v /tmp:/tmp \
		-v /var/run/docker.sock:/var/run/docker.sock \
		--network host \
		$(APP_NAME)

dev:
	docker compose -f docker-compose.dev.yaml up --build

stop:
	docker compose -f docker-compose.dev.yaml down

