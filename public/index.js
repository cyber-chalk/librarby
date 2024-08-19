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
			}
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
