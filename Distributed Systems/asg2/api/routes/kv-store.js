const express = require("express");
const router = express.Router();
const data = require("../../data");

router.get("/:key", (req, res) => {
    const key = req.params.key;
    const currentItem = data.find(item => item.hasOwnProperty(key));

    if (currentItem) {
        res.status(200).json({
            doesExist: true,
            message: "Retrieved successfully",
            value: currentItem[key],
        });
    } else {
        res.status(404).json({
            doesExist: false,
            error: "Key does not exist",
            message: "Error in GET",
        });
    }
});

router.put("/:key", (req, res) => {
    const key = req.params.key;

    if (key.length > 50) {
        res.status(400).json({
            error: "Key is too long",
            message: "Error in PUT",
        });
    } else if (req.body && req.body.value) {
        const value = req.body.value;
        const currentItem = data.find(item => item.hasOwnProperty(key));

        if (currentItem) {
            currentItem[key] = value;
            res.status(200).json({
                message: "Updated successfully",
                replaced: true,
            });
        } else {
            data.push({
                [key]: value
            });
            res.status(201).json({
                message: "Added successfully",
                replaced: false,
            });
        }
    } else {
        res.status(400).json({
            error: "Value is missing",
            message: "Error in PUT",
        });
    }
});

router.delete("/:key", (req, res) => {
    const key = req.params.key;
    const currentItem = data.find(item => item.hasOwnProperty(key));

    if (currentItem) {
        const index = data.indexOf(currentItem);
        data.splice(index, 1);

        res.status(200).json({
            doesExist: true,
            message: "Deleted successfully"
        });
    } else {
        res.status(404).json({
            doesExist: false,
            error: "Key does not exist",
            message: "Error in DELETE",
        });
    }
});

module.exports = router;
