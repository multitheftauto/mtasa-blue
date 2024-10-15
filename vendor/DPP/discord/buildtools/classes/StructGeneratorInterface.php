<?php

namespace Dpp;

/**
 * Represents a header/cpp generator used to auto-generate cpp/.h files.
 */
interface StructGeneratorInterface
{
    /**
     * Generate the start of the header file
     *
     * @return string header content
     */
    public function generateHeaderStart(): string;

    /**
     * Generate the start of the cpp file
     *
     * @return string cpp content
     */
    public function generateCppStart(): string;

    /**
     * Check if the script should run and re-generate content or not
     *
     * @return string true if the script should run, false to exit
     */
    public function checkForchanges(): bool;

    /**
     * Generate header definition for a function
     *
     * @param string $returnType Return type of function
     * @param string $currentFunction Current function name
     * @param string $parameters Current function parameters with default values
     * @param string $noDefaults Current function parameters without default values
     * @param string $parameterNames Parameter names only
     * @return string header content to append
     */
    public function generateHeaderDef(string $returnType, string $currentFunction, string $parameters, string $noDefaults, string $parameterTypes, string $parameterNames): string;

    /**
     * Generate cpp definition for a function
     *
     * @param string $returnType Return type of function
     * @param string $currentFunction Current function name
     * @param string $parameters Current function parameters with default values
     * @param string $noDefaults Current function parameters without default values
     * @param string $parameterNames Parameter names only
     * @return string cpp content to append
     */
    public function generateCppDef(string $returnType, string $currentFunction, string $parameters, string $noDefaults, string $parameterTypes, string $parameterNames): string;
    
    /**
     * Return comment lines to add to each header definition
     *
     * @return array Comment lines to add
     */
    public function getCommentArray(): array;

    /**
     * Save the .h file
     *
     * @param string $content Content to save
     */
    public function saveHeader(string $content): void;

    /**
     * Save the .cpp file
     *
     * @param string $cppcontent Content to save
     */
    public function saveCpp(string $cppcontent): void;
};