package main

import (
	"fmt"
	"net/http"
	"os"
	"strconv"
	"sync"
)

func main() {
	if len(os.Args) != 3 {
		fmt.Println("Error: invalid number of arguments!")
		fmt.Println("You can execute with: ./stress_test {number_of_requests} {serverURL}")
		os.Exit(1)
	}

	requests, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println("Error: invalid number of requests!")
		os.Exit(1)
	}

	server := os.Args[2]
	var wg sync.WaitGroup
	for i := 0; i < requests; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			resp, err := http.Get(server)
			if err != nil {
				fmt.Printf("Request %d failed: %s\n", id, err)
				return
			}
			defer resp.Body.Close()

			fmt.Printf("Request %d completed with status: %s\n", id, resp.Status)
		}(i)
	}
	wg.Wait()
	fmt.Println("Stress test completed!")
}
