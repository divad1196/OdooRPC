#! /bin/bash

# need jq: apt-get install jq

# Ok, return a dict
_authenticate() {
    data="{
        \"jsonrpc\": \"2.0\",
        \"id\": null,
        \"method\": \"call\",
        \"params\": {
            \"db\": \"$2\",
            \"login\": \"$3\",
            \"password\": \"$4\"
        }
    }"
    curl    -X POST \
            -H 'Content-Type: application/json' \
            --data $data \
            --url "$1/web/session/authenticate" \
            -sL
    
}

authenticate() {
    echo $(_authenticate $@ | jq ".result.uid")
}


# OK
common() {
    data="{
        \"jsonrpc\": \"2.0\",
        \"id\": 65663464,
        \"method\": \"call\",
        \"params\": {
            \"service\": \"common\",
            \"method\": \"login\",
            \"args\": [
                \"$2\", \"$3\", \"$4\"
            ]
        }
    }"

    print $data
    curl    -X POST \
            -H 'Content-Type: application/json' \
            --data $data \
            --url "$1/jsonrpc" \
            -L
}


# Ok, use the uid instead of login
raw_query() {
    user_id=$(authenticate $1 $2 $3 $4)
    echo $user_id
    data="{
        \"jsonrpc\": \"2.0\",
        \"id\": null,
        \"method\": \"call\",
        \"params\": {
            \"service\": \"object\",
            \"method\": \"execute\",
            \"args\": [
                \"$2\",
                $user_id,
                \"$4\",
                \"res.partner\",
                \"search\",
                []

            ]
        }
    }"
    print $data
    curl    -X POST \
            -H 'Content-Type: application/json' \
            --data $data \
            --url "$1/jsonrpc" \
            -L
}