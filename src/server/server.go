package main

import (
        "fmt"
        "io"
        "net/http"
)

func get(w http.ResponseWriter, r *http.Request) {
}

func put(w http.ResponseWriter, r *http.Request) {
}

func stat(w http.ResponseWriter, r *http.Request) {
        r.ParseForm()
        fmt.Printf("file=%s", r.Form.Get("file"))
        io.WriteString(w, "100 FOUND")
}

func main() {
        http.HandleFunc("/get", get)
        http.HandleFunc("/put", put)
        http.HandleFunc("/stat", stat)
        http.ListenAndServe(":8000", nil)
}

