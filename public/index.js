function yuck() {
	let rows = document.getElementsByClassName("book-item");
	let grid = document.getElementById("book-container");

	Array.from(rows).forEach((element, i) => {
		element.addEventListener("mouseover", () => {
			let row = Math.floor(i / 5);

			for (let n = 0; n < rows.length; n++) {
				if (Math.floor(n / 5) === row) {
					Array.from(rows[n].getElementsByTagName("img")).forEach(
						(img) => {
							img.style.visibility = "visible";
						}
					);
				} // yuck
			}
		});

		element.addEventListener("mouseout", () => {
			let row = Math.floor(i / 5);

			for (let n = 0; n < rows.length; n++) {
				if (Math.floor(n / 5) === row) {
					Array.from(rows[n].getElementsByTagName("img")).forEach(
						(img) => {
							img.style.visibility = "hidden";
						}
					);
				}
			}
		});
	});
}

function writeList(param) {
	param = param.toLowerCase();
	if (param == "return") param = "return_date";
	fetch(`data.csv?sort=${param}`)
		.then((response) => response.text())
		.then((data) => {
			// console.log(data.toString());
			let rows = data.split("\n");
			let pRows = rows.map((element) => {
				return element.split(",");
			});
			// console.log(pRows);
			// clear list
			let items = document.querySelectorAll(".book-item:not(.head)");
			items.forEach((element) => {
				element.remove();
			});

			// add (i know i could simplify this but i wont)
			for (let i = 0; i < pRows.length; i++) {
				// console.log(pRows[i]);
				for (let j = 0; j < pRows[i].length; j++) {
					let container = document.getElementById("book-container");
					let el = document.createElement("div");

					el.classList.add("book-item");

					container.append(el);
					if (j == pRows[i].length - 1) {
						el.classList.add("condition");
						el.innerHTML = `
    					<img src="borrowIcon.png" alt="Borrow Icon" />
   						<img src="returnIcon.png" alt="Return Icon" />
    					${pRows[i][j]}
   						<img src="remove.png" alt="Remove Icon" />`;
					} else {
						el.innerText = pRows[i][j];
					}
				}
			}
			yuck();
		})
		.catch((error) => {
			console.error("error fetching:", error);
		});
}

writeList("popularity");
