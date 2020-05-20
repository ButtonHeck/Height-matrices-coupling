#pragma once

#include <vector>

enum class COMPARISON_SIDE
{
    LEFT, RIGHT, TOP, BOTTOM
};

/**
 * @brief Height matrix class represented by a 2D vector of height values.
 * matrix data is accessed via iterators
 */
class HeightMatrix
{
public:
    constexpr static float MAX_HEIGHT = 2.0f;

    enum MATRIX_TYPE
    {
        MASTER, TARGET
    };

    static COMPARISON_SIDE sideFrom( int side );

    /**
     * @brief Base class for all types of matrix iterators
     */
    class Iterator
    {
    public:
        Iterator( size_t endIndex );
        bool isValid();
        size_t getCurrentIndex() const;
        virtual float operator++(int) = 0;
        virtual float operator--(int) = 0;

    protected:
        size_t currentIndex;
        size_t endIndex;
    };

    //RowIterator declaration
    class RowIterator : public Iterator
    {
    public:
        RowIterator( const size_t ROW,
                     std::vector< std::vector<float> > & storage );
        float & operator*();
        float operator++(int) override;
        float operator--(int) override;

    private:
        std::vector<float>::iterator iter;
    };

    //ConstRowIterator declaration
    class ConstRowIterator : public Iterator
    {
    public:
        ConstRowIterator( const size_t ROW,
                          const std::vector< std::vector<float> > & STORAGE );
        const float & operator*() const;
        float operator++(int) override;
        float operator--(int) override;

    private:
        std::vector<float>::const_iterator iter;
    };

    //ColumnIterator declaration
    class ColumnIterator : public Iterator
    {
    public:
        ColumnIterator( const size_t COLUMN,
                        std::vector< std::vector<float> > & storage );
        float & operator*();
        float operator++(int) override;
        float operator--(int) override;

    private:
        std::vector< std::vector<float> >::iterator iter;
        size_t column;
    };

    //ConstColumnIterator declaration
    class ConstColumnIterator : public Iterator
    {
    public:
        ConstColumnIterator( const size_t COLUMN,
                             const std::vector< std::vector<float> > & STORAGE );
        const float & operator*() const;
        float operator++(int) override;
        float operator--(int) override;

    private:
        std::vector< std::vector<float> >::const_iterator iter;
        size_t column;
    };

public:
    HeightMatrix( size_t width,
                  size_t height,
                  double precision,
                  MATRIX_TYPE type );
    RowIterator rowBegin( const size_t ROW );
    ConstRowIterator rowBegin( const size_t ROW ) const;
    ColumnIterator columnBegin( const size_t COLUMN );
    ConstColumnIterator columnBegin( const size_t COLUMN ) const;
    size_t getWidth() const;
    size_t getHeight() const;
    double getPrecision() const;
    MATRIX_TYPE getType() const;

private:
    std::vector< std::vector<float> > storage;
    size_t width;
    size_t height;
    double precision;
    MATRIX_TYPE type;
};
